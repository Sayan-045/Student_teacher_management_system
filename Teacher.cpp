


// #include "Teacher.h"
// #include <iostream>
// #include <limits>

// Teacher::Teacher(std::string id, std::string name) : User(id, name) {}

// void Teacher::dashboard(Database& db, const std::string& course_id) {
//     int choice;

//     while (true) {
//         std::cout << "\n--- Teacher Dashboard (" << course_id << ") ---\n";
//         std::cout << "1. View Students\n2. Mark Attendance\n3. Course Report\n4. Exit\nChoice: ";

//         std::cin >> choice;

//         if (std::cin.fail()) {
//             std::cin.clear();
//             std::cin.ignore(10000, '\n');
//             continue;
//         }

//         if (choice == 1) db.viewStudentsInCourse(course_id);
//         else if (choice == 2) {
//             int day;
//             std::cout << "Enter Day: ";
//             std::cin >> day;
//             db.markAttendance(course_id, day);
//         }
//         else if (choice == 3) db.viewCourseReport(course_id);
//         else if (choice == 4) break;
//     }
// }


#include "teacherdashboard.h"
#include "ui_teacherdashboard.h"

#include <QInputDialog>
#include <QMessageBox>

TeacherDashboard::TeacherDashboard(std::string course, Database* db, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TeacherDashboard)
{
    ui->setupUi(this);

    this->course_id = course;
    this->db = db;
}

TeacherDashboard::~TeacherDashboard()
{
    delete ui;
}

// 👥 View Students
void TeacherDashboard::on_viewStudentsBtn_clicked()
{
    db->viewStudentsInCourse(course_id);

    QMessageBox::information(this, "Students", "Check console output");
}

// 🗓️ Mark Attendance
void TeacherDashboard::on_markAttendanceBtn_clicked()
{
    int day = QInputDialog::getInt(this, "Day", "Enter Day:");

    db->markAttendance(course_id, day);

    QMessageBox::information(this, "Done", "Attendance Marked!");
}

// 📝 Add Marks
void TeacherDashboard::on_addMarksBtn_clicked()
{
    db->addMarks(course_id);

    QMessageBox::information(this, "Done", "Marks Updated!");
}

// 📊 Course Report
void TeacherDashboard::on_reportBtn_clicked()
{
    db->viewCourseReport(course_id);

    QMessageBox::information(this, "Report", "Check console output");
}