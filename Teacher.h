#ifndef TEACHERDASHBOARD_H
#define TEACHERDASHBOARD_H

#include <QMainWindow>
#include "Database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TeacherDashboard; }
QT_END_NAMESPACE

class TeacherDashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit TeacherDashboard(std::string course, Database* db, QWidget *parent = nullptr);
    ~TeacherDashboard();

private slots:
    void on_viewStudentsBtn_clicked();
    void on_markAttendanceBtn_clicked();
    void on_addMarksBtn_clicked();
    void on_courseReportBtn_clicked();

private:
    Ui::TeacherDashboard *ui;

    std::string course_id;
    Database* db;
};

#endif