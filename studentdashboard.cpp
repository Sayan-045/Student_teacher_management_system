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
    QString course = QInputDialog::getText(this, "Course", "Enter Course ID:");
    db->enrollStudent(student_id, course.toStdString());
}

void StudentDashboard::on_viewAttendanceBtn_clicked()
{
    db->viewStudentAttendance(student_id);
}

void StudentDashboard::on_reportBtn_clicked()
{
    db->generateReportCard(student_id);
}