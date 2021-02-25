#include "LoginWidget.h"



LoginWidget::LoginWidget()
{
    //init username and password
    username = "unknow";
    password = "unknow";
}

void LoginWidget::InitWidget() {
    QLabel* widgetNameLabel = new QLabel(this);
    QLineEdit* pUserLineEdit = new QLineEdit(this);
    QLineEdit* pPasswordLineEdit = new QLineEdit(this);
    QCheckBox* pRememberCheckBox = new QCheckBox(this);
    QCheckBox* pAutoLoginCheckBox = new QCheckBox(this);
    QPushButton* pLoginButton = new QPushButton(this);
    QLabel* pRegisterLabel = new QLabel(this);
    QLabel* pForgotLabel = new QLabel(this);

    pLoginButton->setFixedHeight(30);
    pUserLineEdit->setFixedWidth(200);

    // 设置头像
    /*QPixmap pixmap(":/Images/logo");
    pImageLabel->setFixedSize(90, 90);
    pImageLabel->setPixmap(pixmap);
    pImageLabel->setScaledContents(true);*/

    // 设置文本
    widgetNameLabel->setText(QStringLiteral("账号密码登录"));
    pUserLineEdit->setPlaceholderText(QStringLiteral("QQ号码/手机/邮箱"));
    pPasswordLineEdit->setPlaceholderText(QStringLiteral("密码"));
    pPasswordLineEdit->setEchoMode(QLineEdit::Password);
    pRememberCheckBox->setText(QStringLiteral("记住密码"));
   /* pAutoLoginCheckBox->setText(QStringLiteral("自动登录"));*/
    pLoginButton->setText(QStringLiteral("登录"));
    pRegisterLabel->setText(QStringLiteral("新用户注册"));
    pForgotLabel->setText(QStringLiteral("忘记密码?"));

    QGridLayout* pLayout = new QGridLayout();
    // widget name 第1行，第0列开始，占2行2列
    pLayout->addWidget(widgetNameLabel, 1, 0, 2, 2);
    // 用户名输入框 第3行，第0列开始，占1行3列
    pLayout->addWidget(pUserLineEdit, 3, 0, 1, 3);
    /*pLayout->addWidget(pRegisterLabel, 5, 0, 1, 2);*/
    // 密码输入框 第1行，第1列开始，占1行2列
    pLayout->addWidget(pPasswordLineEdit, 5, 0, 1, 2);
    pLayout->addWidget(pForgotLabel, 5, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    // 登录按钮 第3行，第1列开始，占1行2列
    pLayout->addWidget(pLoginButton, 7, 0, 1, 3);

    // 记住密码 第2行，第1列开始，占1行1列 水平居左 垂直居中
    pLayout->addWidget(pRememberCheckBox, 8, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    pLayout->addWidget(pRegisterLabel, 8, 0, 1, 3, Qt::AlignLeft | Qt::AlignVCenter);
    //// 自动登录 第2行，第2列开始，占1行1列 水平居右 垂直居中
    //pLayout->addWidget(pAutoLoginCheckBox, 2, 2, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    // 设置水平间距
    pLayout->setHorizontalSpacing(10);
    // 设置垂直间距
    pLayout->setVerticalSpacing(10);
    // 设置外间距
    pLayout->setContentsMargins(10, 10, 10, 10);
    setLayout(pLayout);
}
LoginWidget::~LoginWidget()
{
}

bool LoginWidget::UserIsValid(QString username, QString password)
{
    return false;
}
