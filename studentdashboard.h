#ifndef STUDENTDASHBOARD_H
#define STUDENTDASHBOARD_H

#include <QMainWindow>
#include <string>
#include "Database.h"   // ✅ IMPORTANT

QT_BEGIN_NAMESPACE
namespace Ui { class StudentDashboard; }
QT_END_NAMESPACE

class StudentDashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit StudentDashboard(std::string id, Database* db, QWidget *parent = nullptr);
    ~StudentDashboard();

private slots:
    void on_addCourseBtn_clicked();
    void on_viewAttendanceBtn_clicked();
    void on_reportBtn_clicked();

private:
    Ui::StudentDashboard *ui;

    // ✅ THESE WERE MISSING
    std::string student_id;
    Database* db;
};

#endif // STUDENTDASHBOARD_H
