

// #ifndef DATABASE_H
// #define DATABASE_H

// #include "sqlite3.h"
// #include <string>

// class Database {
// private:
//     sqlite3* DB;

// public:
//     Database();
//     ~Database();

//     bool execute(const std::string& sql);

//     bool userExists(const std::string& table, const std::string& id);
//     void insertUser(const std::string& table, const std::string& id, const std::string& name);

//     bool courseExists(const std::string& course_id);
//     void addCourse(const std::string& course_id, int total_classes);

//     void enrollStudent(const std::string& student_id, const std::string& course_id);

//     void markAttendance(const std::string& course_id, int day);

//     void viewStudentsInCourse(const std::string& course_id);

//     void viewStudentAttendance(const std::string& student_id);
//     void viewStudentCourses(const std::string& student_id);
//     void generateReportCard(const std::string& student_id);

//     void viewCourseReport(const std::string& course_id);
// };

// #endif

#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"
#include <string>

class Database {
private:
    sqlite3* DB;

public:
    Database();
    ~Database();

    bool execute(const std::string& sql);

    bool userExists(const std::string& table, const std::string& id);
    void insertUser(const std::string& table, const std::string& id, const std::string& name);

    bool courseExists(const std::string& course_id);
    void addCourse(const std::string& course_id, int total_classes);

    void enrollStudent(const std::string& student_id, const std::string& course_id);

    void markAttendance(const std::string& course_id, int day);

    void addMarks(const std::string& course_id);

    void viewStudentsInCourse(const std::string& course_id);

    void viewStudentAttendance(const std::string& student_id);
    void viewStudentCourses(const std::string& student_id);
    void generateReportCard(const std::string& student_id);

    void viewCourseReport(const std::string& course_id);

    // NEW GUI FUNCTIONS

    void addAttendance(const std::string& student_id,
                       const std::string& course_id,
                       int day,
                       int present);

    void setMarks(const std::string& student_id,
                  const std::string& course_id,
                  int marks);
};

#endif