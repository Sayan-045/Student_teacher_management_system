
// #include "Database.h"
// #include <iostream>

// Database::Database() {
//     sqlite3_open("portal.db", &DB);

//     execute("CREATE TABLE IF NOT EXISTS students(id TEXT PRIMARY KEY, name TEXT);");

//     execute("CREATE TABLE IF NOT EXISTS courses(course_id TEXT PRIMARY KEY, total_classes INT);");

//     execute("CREATE TABLE IF NOT EXISTS enrollments(student_id TEXT, course_id TEXT);");

//     execute("CREATE TABLE IF NOT EXISTS attendance(student_id TEXT, course_id TEXT, day INT, present INT);");

//     execute("CREATE TABLE IF NOT EXISTS marks(student_id TEXT, course_id TEXT, marks INT);");
// }

// Database::~Database() {
//     sqlite3_close(DB);
// }

// bool Database::execute(const std::string& sql) {
//     char* errMsg;
//     if (sqlite3_exec(DB, sql.c_str(), NULL, NULL, &errMsg) != SQLITE_OK) {
//         std::cout << "SQL Error: " << errMsg << "\n";
//         sqlite3_free(errMsg);
//         return false;
//     }
//     return true;
// }

// // ---------------- USER ----------------

// bool Database::userExists(const std::string& table, const std::string& id) {
//     std::string sql = "SELECT * FROM " + table + " WHERE id='" + id + "';";
//     sqlite3_stmt* stmt;

//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
//     bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
//     sqlite3_finalize(stmt);

//     return exists;
// }

// void Database::insertUser(const std::string& table, const std::string& id, const std::string& name) {
//     execute("INSERT INTO " + table + " VALUES('" + id + "','" + name + "');");
// }

// // ---------------- COURSE ----------------

// bool Database::courseExists(const std::string& course_id) {
//     std::string sql = "SELECT * FROM courses WHERE course_id='" + course_id + "';";
//     sqlite3_stmt* stmt;

//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
//     bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
//     sqlite3_finalize(stmt);

//     return exists;
// }

// void Database::addCourse(const std::string& course_id, int total_classes) {
//     if (courseExists(course_id)) {
//         std::cout << "Course already exists!\n";
//         return;
//     }

//     execute("INSERT INTO courses VALUES('" + course_id + "'," + std::to_string(total_classes) + ");");
//     std::cout << "Course created!\n";
// }

// // ---------------- ENROLL ----------------

// void Database::enrollStudent(const std::string& student_id, const std::string& course_id) {
//     if (!courseExists(course_id)) {
//         std::cout << "Course does not exist!\n";
//         return;
//     }

//     execute("INSERT INTO enrollments VALUES('" + student_id + "','" + course_id + "');");
//     std::cout << "Enrolled successfully!\n";
// }

// // ---------------- ATTENDANCE ----------------

// void Database::markAttendance(const std::string& course_id, int day) {
//     std::string sql = "SELECT student_id FROM enrollments WHERE course_id='" + course_id + "';";
//     sqlite3_stmt* stmt;

//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         std::string sid = (const char*)sqlite3_column_text(stmt, 0);

//         int p;
//         std::cout << "Student " << sid << " (1=Present,0=Absent): ";
//         std::cin >> p;

//         execute("INSERT INTO attendance VALUES('" + sid + "','" + course_id + "'," +
//                 std::to_string(day) + "," + std::to_string(p) + ");");
//     }

//     sqlite3_finalize(stmt);
// }

// // ---------------- MARKS ----------------

// void Database::addMarks(const std::string& course_id) {
//     std::string sql = "SELECT student_id FROM enrollments WHERE course_id='" + course_id + "';";
//     sqlite3_stmt* stmt;

//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         std::string sid = (const char*)sqlite3_column_text(stmt, 0);

//         int m;
//         std::cout << "Marks for " << sid << ": ";
//         std::cin >> m;

//         execute("INSERT OR REPLACE INTO marks VALUES('" + sid + "','" + course_id + "'," +
//                 std::to_string(m) + ");");
//     }

//     sqlite3_finalize(stmt);
// }

// // ---------------- VIEW STUDENTS ----------------

// void Database::viewStudentsInCourse(const std::string& course_id) {
//     std::string sql =
//     "SELECT s.id, s.name FROM students s "
//     "JOIN enrollments e ON s.id = e.student_id "
//     "WHERE e.course_id='" + course_id + "';";

//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

//     std::cout << "\n--- Students in Course ---\n";

//     bool found = false;

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         found = true;
//         std::cout << "ID: " << sqlite3_column_text(stmt, 0)
//                   << " Name: " << sqlite3_column_text(stmt, 1) << "\n";
//     }

//     if (!found) std::cout << "No students found!\n";

//     sqlite3_finalize(stmt);
// }

// // ---------------- STUDENT ATTENDANCE ----------------

// void Database::viewStudentAttendance(const std::string& student_id) {
//     std::string sql =
//     "SELECT c.course_id, c.total_classes, COUNT(a.present), SUM(a.present) "
//     "FROM courses c "
//     "LEFT JOIN attendance a ON c.course_id = a.course_id AND a.student_id='" + student_id + "' "
//     "JOIN enrollments e ON c.course_id = e.course_id "
//     "WHERE e.student_id='" + student_id + "' "
//     "GROUP BY c.course_id;";

//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

//     std::cout << "\n--- Attendance ---\n";

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         std::string course = (const char*)sqlite3_column_text(stmt, 0);

//         int total_classes = sqlite3_column_int(stmt, 1);
//         int total_taken = sqlite3_column_int(stmt, 2);
//         int present = sqlite3_column_int(stmt, 3);

//         float percent = (total_taken == 0) ? 0 : (present * 100.0 / total_taken);

//         int needed = 0;
//         while (true) {
//             float new_percent = ((present + needed) * 100.0) / (total_taken + needed);
//             if (new_percent >= 75.0 || (total_taken + needed) >= total_classes) break;
//             needed++;
//         }

//         std::cout << "Course: " << course
//                   << " | Attendance: " << percent
//                   << "% | Need " << needed << " classes for 75%\n";
//     }

//     sqlite3_finalize(stmt);
// }

// // ---------------- REPORT CARD ----------------

// void Database::generateReportCard(const std::string& student_id) {
//     std::string sql =
//     "SELECT e.course_id, COUNT(a.present), SUM(a.present), m.marks "
//     "FROM enrollments e "
//     "LEFT JOIN attendance a ON e.student_id=a.student_id AND e.course_id=a.course_id "
//     "LEFT JOIN marks m ON e.student_id=m.student_id AND e.course_id=m.course_id "
//     "WHERE e.student_id='" + student_id + "' GROUP BY e.course_id;";

//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

//     std::cout << "\n--- Report Card ---\n";

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         std::string c = (const char*)sqlite3_column_text(stmt, 0);
//         int total = sqlite3_column_int(stmt, 1);
//         int present = sqlite3_column_int(stmt, 2);
//         int marks = sqlite3_column_int(stmt, 3);

//         float percent = (total == 0) ? 0 : present * 100.0 / total;

//         char grade = 'D';
//         if (marks >= 90) grade = 'A';
//         else if (marks >= 75) grade = 'B';
//         else if (marks >= 60) grade = 'C';

//         std::cout << c
//                   << " | Attendance: " << percent
//                   << "% | Marks: " << marks
//                   << " | Grade: " << grade << "\n";
//     }

//     sqlite3_finalize(stmt);
// }

// // ---------------- COURSE REPORT ----------------

// void Database::viewCourseReport(const std::string& course_id) {
//     std::string sql =
//     "SELECT s.id, s.name, COUNT(a.present), SUM(a.present), m.marks "
//     "FROM enrollments e "
//     "JOIN students s ON e.student_id = s.id "
//     "LEFT JOIN attendance a ON e.student_id = a.student_id AND e.course_id = a.course_id "
//     "LEFT JOIN marks m ON e.student_id = m.student_id AND e.course_id = m.course_id "
//     "WHERE e.course_id='" + course_id + "' GROUP BY s.id;";

//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

//     std::cout << "\n--- Course Report ---\n";

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         std::string id = (const char*)sqlite3_column_text(stmt, 0);
//         std::string name = (const char*)sqlite3_column_text(stmt, 1);

//         int total = sqlite3_column_int(stmt, 2);
//         int present = sqlite3_column_int(stmt, 3);
//         int marks = sqlite3_column_int(stmt, 4);

//         float percent = (total == 0) ? 0 : present * 100.0 / total;

//         char grade = 'D';
//         if (marks >= 90) grade = 'A';
//         else if (marks >= 75) grade = 'B';
//         else if (marks >= 60) grade = 'C';

//         std::cout << "ID: " << id
//                   << " Name: " << name
//                   << " | Present: " << present
//                   << " | Total: " << total
//                   << " | %: " << percent
//                   << " | Marks: " << marks
//                   << " | Grade: " << grade << "\n";
//     }

//     sqlite3_finalize(stmt);
// }




#include "Database.h"
#include <iostream>

Database::Database() {
    sqlite3_open("portal.db", &DB);

    execute("CREATE TABLE IF NOT EXISTS students(id TEXT PRIMARY KEY, name TEXT);");

    execute("CREATE TABLE IF NOT EXISTS courses(course_id TEXT PRIMARY KEY, total_classes INT);");

    execute("CREATE TABLE IF NOT EXISTS enrollments(student_id TEXT, course_id TEXT);");

    execute("CREATE TABLE IF NOT EXISTS attendance(student_id TEXT, course_id TEXT, day INT, present INT);");

    execute("CREATE TABLE IF NOT EXISTS marks(student_id TEXT, course_id TEXT, marks INT);");
}

Database::~Database() {
    sqlite3_close(DB);
}

// ---------------- BASIC EXEC ----------------

bool Database::execute(const std::string& sql) {
    char* errMsg;
    if (sqlite3_exec(DB, sql.c_str(), NULL, NULL, &errMsg) != SQLITE_OK) {
        std::cout << "SQL Error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ---------------- USER ----------------

bool Database::userExists(const std::string& table, const std::string& id) {
    std::string sql = "SELECT * FROM " + table + " WHERE id='" + id + "';";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    return exists;
}

void Database::insertUser(const std::string& table, const std::string& id, const std::string& name) {
    execute("INSERT INTO " + table + " VALUES('" + id + "','" + name + "');");
}

// ---------------- COURSE ----------------

bool Database::courseExists(const std::string& course_id) {
    std::string sql = "SELECT * FROM courses WHERE course_id='" + course_id + "';";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    return exists;
}

void Database::addCourse(const std::string& course_id, int total_classes) {
    if (courseExists(course_id)) return;

    execute("INSERT INTO courses VALUES('" + course_id + "'," + std::to_string(total_classes) + ");");
}

// ---------------- ENROLL ----------------

void Database::enrollStudent(const std::string& student_id, const std::string& course_id) {
    if (!courseExists(course_id)) return;

    execute("INSERT INTO enrollments VALUES('" + student_id + "','" + course_id + "');");
}

// ---------------- ATTENDANCE (NEW GUI BASED) ----------------

// void Database::markAttendance(const std::string& student_id,
//                              const std::string& course_id,
//                              int day,
//                              int present)
//
// {
//     execute("INSERT INTO attendance VALUES('" + student_id + "','" +
//             course_id + "'," + std::to_string(day) + "," +
//             std::to_string(present) + ");");
// }
void Database::markAttendance(const std::string& course_id, int day) {
    std::string sql = "SELECT student_id FROM enrollments WHERE course_id='" + course_id + "';";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string sid = (const char*)sqlite3_column_text(stmt, 0);

        int p;
        std::cout << "Student " << sid << " (1=Present,0=Absent): ";
        std::cin >> p;

        execute("INSERT INTO attendance VALUES('" + sid + "','" + course_id + "'," +
                std::to_string(day) + "," + std::to_string(p) + ");");
    }

    sqlite3_finalize(stmt);
}

// ---------------- MARKS (NEW GUI BASED) ----------------

// void Database::setMarks(const std::string& student_id,
//                         const std::string& course_id,
//                         int marks)
// {
//     execute("INSERT OR REPLACE INTO marks VALUES('" +
//             student_id + "','" + course_id + "'," +
//             std::to_string(marks) + ");");
// }

void Database::addMarks(const std::string& course_id) {
    std::string sql = "SELECT student_id FROM enrollments WHERE course_id='" + course_id + "';";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string sid = (const char*)sqlite3_column_text(stmt, 0);

        int m;
        std::cout << "Marks for " << sid << ": ";
        std::cin >> m;

        execute("INSERT OR REPLACE INTO marks VALUES('" + sid + "','" + course_id + "'," +
                std::to_string(m) + ");");
    }

    sqlite3_finalize(stmt);
}

// ---------------- VIEW STUDENTS ----------------

void Database::viewStudentsInCourse(const std::string& course_id) {
    std::string sql =
        "SELECT s.id, s.name FROM students s "
        "JOIN enrollments e ON s.id = e.student_id "
        "WHERE e.course_id='" + course_id + "';";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::cout << "\n--- Students in Course ---\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << "ID: " << sqlite3_column_text(stmt, 0)
        << " Name: " << sqlite3_column_text(stmt, 1) << "\n";
    }

    sqlite3_finalize(stmt);
}

// ---------------- STUDENT ATTENDANCE ----------------

void Database::viewStudentAttendance(const std::string& student_id) {
    std::string sql =
        "SELECT c.course_id, c.total_classes, COUNT(a.present), SUM(a.present) "
        "FROM courses c "
        "LEFT JOIN attendance a ON c.course_id = a.course_id AND a.student_id='" + student_id + "' "
                       "JOIN enrollments e ON c.course_id = e.course_id "
                       "WHERE e.student_id='" + student_id + "' "
                       "GROUP BY c.course_id;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::cout << "\n--- Attendance ---\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string course = (const char*)sqlite3_column_text(stmt, 0);

        int total_classes = sqlite3_column_int(stmt, 1);
        int total_taken = sqlite3_column_int(stmt, 2);
        int present = sqlite3_column_int(stmt, 3);

        float percent = (total_taken == 0) ? 0 : (present * 100.0 / total_taken);

        int needed = 0;
        while (true) {
            float new_percent = ((present + needed) * 100.0) / (total_taken + needed);
            if (new_percent >= 75.0 || (total_taken + needed) >= total_classes) break;
            needed++;
        }

        std::cout << "Course: " << course
                  << " | Attendance: " << percent
                  << "% | Need " << needed << " classes for 75%\n";
    }

    sqlite3_finalize(stmt);
}

// ---------------- REPORT CARD ----------------

void Database::generateReportCard(const std::string& student_id) {
    std::string sql =
        "SELECT e.course_id, COUNT(a.present), SUM(a.present), m.marks "
        "FROM enrollments e "
        "LEFT JOIN attendance a ON e.student_id=a.student_id AND e.course_id=a.course_id "
        "LEFT JOIN marks m ON e.student_id=m.student_id AND e.course_id=m.course_id "
        "WHERE e.student_id='" + student_id + "' GROUP BY e.course_id;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::cout << "\n--- Report Card ---\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string c = (const char*)sqlite3_column_text(stmt, 0);
        int total = sqlite3_column_int(stmt, 1);
        int present = sqlite3_column_int(stmt, 2);
        int marks = sqlite3_column_int(stmt, 3);

        float percent = (total == 0) ? 0 : present * 100.0 / total;

        char grade = 'D';
        if (marks >= 90) grade = 'A';
        else if (marks >= 75) grade = 'B';
        else if (marks >= 60) grade = 'C';

        std::cout << c
                  << " | Attendance: " << percent
                  << "% | Marks: " << marks
                  << " | Grade: " << grade << "\n";
    }

    sqlite3_finalize(stmt);
}

// ---------------- COURSE REPORT ----------------

void Database::viewCourseReport(const std::string& course_id) {
    std::string sql =
        "SELECT s.id, s.name, COUNT(a.present), SUM(a.present), m.marks "
        "FROM enrollments e "
        "JOIN students s ON e.student_id = s.id "
        "LEFT JOIN attendance a ON e.student_id = a.student_id AND e.course_id = a.course_id "
        "LEFT JOIN marks m ON e.student_id = m.student_id AND e.course_id = m.course_id "
        "WHERE e.course_id='" + course_id + "' GROUP BY s.id;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);

    std::cout << "\n--- Course Report ---\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = (const char*)sqlite3_column_text(stmt, 0);
        std::string name = (const char*)sqlite3_column_text(stmt, 1);

        int total = sqlite3_column_int(stmt, 2);
        int present = sqlite3_column_int(stmt, 3);
        int marks = sqlite3_column_int(stmt, 4);

        float percent = (total == 0) ? 0 : present * 100.0 / total;

        char grade = 'D';
        if (marks >= 90) grade = 'A';
        else if (marks >= 75) grade = 'B';
        else if (marks >= 60) grade = 'C';

        std::cout << "ID: " << id
                  << " Name: " << name
                  << " | Present: " << present
                  << " | Total: " << total
                  << " | %: " << percent
                  << " | Marks: " << marks
                  << " | Grade: " << grade << "\n";
    }

    sqlite3_finalize(stmt);
}