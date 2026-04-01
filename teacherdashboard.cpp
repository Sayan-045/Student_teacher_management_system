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
    int day = QInputDialog::getInt(this, "Day", "Enter Day:");
    db->markAttendance(course_id, day);
    QMessageBox::information(this, "Done", "Attendance Marked!");
}

void TeacherDashboard::on_addMarksBtn_clicked()
{
    db->addMarks(course_id);
    QMessageBox::information(this, "Done", "Marks Updated!");
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
    table->setEditTriggers(QAbstractItemView::NoEditTriggers); // Make read-only

    for (size_t i = 0; i < data.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(data[i].id));
        table->setItem(i, 1, new QTableWidgetItem(data[i].name));

        // Create an interactive visual bar for Attendance
        QProgressBar *attBar = new QProgressBar();
        attBar->setValue(data[i].attPercent);
        attBar->setStyleSheet("QProgressBar::chunk { background-color: #0d6efd; }"); // Blue bar
        table->setCellWidget(i, 2, attBar);

        // Create an interactive visual bar for Marks
        QProgressBar *marksBar = new QProgressBar();
        marksBar->setValue(data[i].marks);
        marksBar->setStyleSheet("QProgressBar::chunk { background-color: #198754; }"); // Green bar
        table->setCellWidget(i, 3, marksBar);

        table->setItem(i, 4, new QTableWidgetItem(data[i].grade));
        table->item(i, 4)->setTextAlignment(Qt::AlignCenter);
    }

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(table);
    dialog.exec();
}
void TeacherDashboard::on_logoutBtn_clicked()
{
    // Create a new login window
    LoginWindow *login = new LoginWindow();
    login->show();

    // Close the current dashboard
    this->close();
}