#include "loginwindow.h"
#include "studentdashboard.h"
#include "ui_studentdashboard.h"
#include <QInputDialog>
#include <QMessageBox>

StudentDashboard::StudentDashboard(std::string id, Database* db, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::StudentDashboard)
{
    this->student_id = id;
    this->db = db;
    ui->setupUi(this);
}

StudentDashboard::~StudentDashboard()
{
    delete ui;
}

void StudentDashboard::on_addCourseBtn_clicked()
{
    QStringList courses = db->getAvailableCourses();
    if (courses.isEmpty()) {
        QMessageBox::information(this, "Empty", "No courses available to register right now.");
        return;
    }

    bool ok;
    // This creates a beautiful dropdown menu!
    QString selected = QInputDialog::getItem(this, "Register", "Select a Course:", courses, 0, false, &ok);

    if (ok && !selected.isEmpty()) {
        db->enrollStudent(student_id, selected.toStdString());
        QMessageBox::information(this, "Success", "Registered for " + selected + "!");
    }
}

// Add this new function
void StudentDashboard::on_dayWiseBtn_clicked()
{
    QStringList courses = db->getEnrolledCourses(student_id);
    if (courses.isEmpty()) {
        QMessageBox::information(this, "Empty", "You are not enrolled in any courses.");
        return;
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "Select Course", "View attendance for:", courses, 0, false, &ok);

    if (ok && !selected.isEmpty()) {
        QString data = db->getDayWiseAttendance(student_id, selected.toStdString());
        QMessageBox::information(this, selected + " Attendance", data);
    }
}

void StudentDashboard::on_viewAttendanceBtn_clicked()
{
    std::string data = db->viewStudentAttendance(student_id);
    QMessageBox::information(this, "Attendance", QString::fromStdString(data));
}

void StudentDashboard::on_reportBtn_clicked()
{
    std::string data = db->generateReportCard(student_id);
    QMessageBox::information(this, "Report Card", QString::fromStdString(data));
}

void StudentDashboard::on_logoutBtn_clicked()
{
    // Create a new login window
    LoginWindow *login = new LoginWindow();
    login->show();

    // Close the current dashboard
    this->close();
}