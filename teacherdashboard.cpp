#include "teacherdashboard.h"
#include "ui_teacherdashboard.h"
#include <QInputDialog>


TeacherDashboard::TeacherDashboard(std::string course, Database* db, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TeacherDashboard)
{
    this->course_id = course;   // ✅ now works
    this->db = db;              // ✅ now works

    ui->setupUi(this);
}

TeacherDashboard::~TeacherDashboard()
{
    delete ui;
}

void TeacherDashboard::on_viewStudentsBtn_clicked()
{
    db->viewStudentsInCourse(course_id);
}

void TeacherDashboard::on_markAttendanceBtn_clicked()
{
    int day = QInputDialog::getInt(this, "Day", "Enter Day:");
    db->markAttendance(course_id, day);
}

void TeacherDashboard::on_addMarksBtn_clicked()
{
    db->addMarks(course_id);
}

void TeacherDashboard::on_reportBtn_clicked()
{
    db->viewCourseReport(course_id);
}

