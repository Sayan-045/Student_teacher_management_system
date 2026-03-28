


// #include "Student.h"
// #include <iostream>

// Student::Student(std::string id, std::string name) : User(id, name) {}

// void Student::dashboard(Database& db) {
//     int ch;

//     while (true) {
//         std::cout << "\n--- Student Dashboard ---\n";
//         std::cout << "1. Add Course\n2. Attendance\n3. Report Card\n4. Exit\n";

//         std::cin >> ch;

//         if (ch == 1) {
//             std::string c;
//             std::cout << "Course ID: ";
//             std::cin >> c;
//             db.enrollStudent(id, c);
//         }
//         else if (ch == 2) db.viewStudentAttendance(id);
//         else if (ch == 3) db.generateReportCard(id);
//         else break;
//     }
// }

#include "studentdashboard.h"
#include "ui_studentdashboard.h"

#include <QInputDialog>
#include <QMessageBox>

StudentDashboard::StudentDashboard(std::string id, Database* db, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::StudentDashboard)
{
    ui->setupUi(this);

    this->student_id = id;
    this->db = db;
}

StudentDashboard::~StudentDashboard()
{
    delete ui;
}

// ➕ Add Course
void StudentDashboard::on_addCourseBtn_clicked()
{
    QString course = QInputDialog::getText(this, "Add Course", "Enter Course ID:");

    if(course.isEmpty()) return;

    db->enrollStudent(student_id, course.toStdString());

    QMessageBox::information(this, "Done", "Course Added!");
}

// 📊 View Attendance
void StudentDashboard::on_viewAttendanceBtn_clicked()
{
    db->viewStudentAttendance(student_id);

    QMessageBox::information(this, "Attendance", "Check console output");
}

// 📄 Report Card
void StudentDashboard::on_reportBtn_clicked()
{
    db->generateReportCard(student_id);

    QMessageBox::information(this, "Report", "Check console output");
}