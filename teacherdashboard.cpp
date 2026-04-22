#include "loginwindow.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QDialog>
#include <QProgressBar>
#include "teacherdashboard.h"
#include "ui_teacherdashboard.h"
#include <QInputDialog>
#include <QMessageBox>

TeacherDashboard::TeacherDashboard(std::string course, Database* db, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TeacherDashboard)
{
    this->course_id = course;
    this->db = db;
    ui->setupUi(this);
}

TeacherDashboard::~TeacherDashboard()
{
    delete ui;
}

void TeacherDashboard::on_viewStudentsBtn_clicked()
{
    auto data = db->getStudentsInCourseData(course_id);

    QDialog dialog(this);
    dialog.setWindowTitle("Enrolled Students");
    dialog.resize(400, 300);

    QTableWidget *table = new QTableWidget(data.size(), 2, &dialog);
    table->setHorizontalHeaderLabels({"Student ID", "Full Name"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (size_t i = 0; i < data.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(data[i].first));
        table->setItem(i, 1, new QTableWidgetItem(data[i].second));
    }

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(table);
    dialog.exec();
}

void TeacherDashboard::on_markAttendanceBtn_clicked()
{
    int nextDay = db->getCurrentDay(course_id) + 1;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "New Attendance",
                                  "Start attendance for Day " + QString::number(nextDay) + "?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) return;

    if (db->markAttendance(course_id, nextDay)) {
        QMessageBox::information(this, "Success", "Attendance for Day " + QString::number(nextDay) + " marked successfully!");
    }
}

void TeacherDashboard::on_addMarksBtn_clicked()
{
    if (db->addMarks(course_id)) {
        QMessageBox::information(this, "Done", "Marks Updated Successfully!");
    }
}

void TeacherDashboard::on_reportBtn_clicked()
{
    auto data = db->getCourseReportData(course_id);

    QDialog dialog(this);
    dialog.setWindowTitle("Interactive Class Performance Report");
    dialog.resize(800, 400);

    QTableWidget *table = new QTableWidget(data.size(), 5, &dialog);
    table->setHorizontalHeaderLabels({"ID", "Name", "Attendance Chart", "Grade Bar", "Final Grade"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (size_t i = 0; i < data.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(data[i].id));
        table->setItem(i, 1, new QTableWidgetItem(data[i].name));


        QProgressBar *attBar = new QProgressBar();
        attBar->setValue(data[i].attPercent);
        attBar->setStyleSheet("QProgressBar::chunk { background-color: #0d6efd; }");
        table->setCellWidget(i, 2, attBar);


        QProgressBar *marksBar = new QProgressBar();
        marksBar->setValue(data[i].marks);
        marksBar->setStyleSheet("QProgressBar::chunk { background-color: #198754; }");
        table->setCellWidget(i, 3, marksBar);

        table->setItem(i, 4, new QTableWidgetItem(data[i].grade));
        table->item(i, 4)->setTextAlignment(Qt::AlignCenter);
    }

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(table);
    dialog.exec();
}

void TeacherDashboard::on_resetPasswordBtn_clicked()
{
    bool ok;

    QStringList resetOptions = {"Reset My Password (Course)", "Reset an Enrolled Student's Password"};
    QString choice = QInputDialog::getItem(this, "Reset Options", "Select an action:", resetOptions, 0, false, &ok);

    if (!ok || choice.isEmpty()) return;

    if (choice == "Reset My Password (Course)") {
        QString newPassword = QInputDialog::getText(this, "Reset Course Password",
                                                    "Enter new password for your course:",
                                                    QLineEdit::Password, "", &ok);
        if (ok && !newPassword.isEmpty()) {
            db->updatePassword("courses", "course_id", course_id, newPassword.toStdString());
            QMessageBox::information(this, "Success", "Your course password was updated successfully!");
        }
    }

    else if (choice == "Reset an Enrolled Student's Password") {

        auto students = db->getStudentsInCourseData(course_id);

        if (students.empty()) {
            QMessageBox::warning(this, "Error", "There are no students enrolled in your course yet.");
            return;
        }

        QStringList studentList;
        for (const auto& s : students) {
            studentList << s.first + " - " + s.second;
        }

        QString selectedStudent = QInputDialog::getItem(this, "Select Student",
                                                        "Choose a student to reset:",
                                                        studentList, 0, false, &ok);

        if (!ok || selectedStudent.isEmpty()) return;

        QString targetId = selectedStudent.split(" - ").first();

        QString newPassword = QInputDialog::getText(this, "Reset Student Password",
                                                    "Enter new password for student " + targetId + ":",
                                                    QLineEdit::Password, "", &ok);

        if (ok && !newPassword.isEmpty()) {
            db->updatePassword("students", "id", targetId.toStdString(), newPassword.toStdString());
            QMessageBox::information(this, "Success", "Student password updated successfully!");
        }
    }
}

void TeacherDashboard::on_updateAttendanceBtn_clicked()
{
    int maxDay = db->getCurrentDay(course_id);

    if (maxDay == 0) {
        QMessageBox::warning(this, "Error", "No attendance records exist yet. Please mark new attendance first.");
        return;
    }

    bool ok;
    int day = QInputDialog::getInt(this, "Update Attendance",
                                   "Enter Day to update (1 to " + QString::number(maxDay) + "):",
                                   maxDay, 1, maxDay, 1, &ok);
    if (!ok) return;

    auto students = db->getStudentsInCourseData(course_id);
    QStringList studentList;
    for (const auto& s : students) {
        studentList << s.first + " - " + s.second;
    }

    QString selectedStudent = QInputDialog::getItem(this, "Select Student",
                                                    "Choose the student to update:",
                                                    studentList, 0, false, &ok);
    if (!ok || selectedStudent.isEmpty()) return;

    QString targetId = selectedStudent.split(" - ").first();

    int status = QInputDialog::getInt(this, "Attendance Status",
                                      "Enter 1 for Present, 0 for Absent:",
                                      1, 0, 1, 1, &ok);
    if (!ok) return;
    if (db->updateSpecificAttendance(course_id, day, targetId.toStdString(), status)) {
        QMessageBox::information(this, "Success", "Attendance record updated successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Could not find a record for that student on that day. Did they enroll after this day?");
    }
}
void TeacherDashboard::on_logoutBtn_clicked()
{
    LoginWindow *login = new LoginWindow();
    login->show();

    this->close();
}