#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>



class LoginWidget : public QWidget {
	Q_OBJECT
public:
	LoginWidget();
	~LoginWidget();

public:
	QPushButton* cfmBtn;
    QLineEdit* userNameEdit;
	QLineEdit* passwordEdit;
	QLabel* widgetNameLabel;
	QGridLayout* widgetLayout;
	

	void InitWidget();
	bool UserIsValid(QString username, QString password);
private: 
	QString username;
	QString password;
};
