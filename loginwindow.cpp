#include "loginwindow.h"
#include "ui_loginwindow.h"
#include"Database.h"
#include <QInputDialog>

#include "studentdashboard.h"
#include "teacherdashboard.h"

#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_loginButton_clicked()
{
    QString role = ui->comboBox->currentText();
    QString id = ui->idEdit->text();

    if(id.isEmpty()){
        QMessageBox::warning(this, "Error", "Enter ID!");
        return;
    }

    if (role == "Student") {

        std::string sid = id.toStdString();

        if (!db.userExists("students", sid)) {

            // Ask for name (SIGNUP)
            bool ok;
            QString name = QInputDialog::getText(this, "Signup",
                                                 "Enter your name:",
                                                 QLineEdit::Normal,
                                                 "", &ok);

            if (!ok || name.isEmpty()) return;

            db.insertUser("students", sid, name.toStdString());

            QMessageBox::information(this, "Success", "Account created!");
        }

        // Open student dashboard
        StudentDashboard *sd = new StudentDashboard(sid,&db);
        sd->show();
        this->close();
    }
    else {
        if (!db.courseExists(id.toStdString())) {
            db.addCourse(id.toStdString(), 30);
        }

        TeacherDashboard *t = new TeacherDashboard(id.toStdString(), &db);
        t->show();
    }

    this->close();
}