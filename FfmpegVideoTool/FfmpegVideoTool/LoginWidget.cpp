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

    // ����ͷ��
    /*QPixmap pixmap(":/Images/logo");
    pImageLabel->setFixedSize(90, 90);
    pImageLabel->setPixmap(pixmap);
    pImageLabel->setScaledContents(true);*/

    // �����ı�
    widgetNameLabel->setText(QStringLiteral("�˺������¼"));
    pUserLineEdit->setPlaceholderText(QStringLiteral("QQ����/�ֻ�/����"));
    pPasswordLineEdit->setPlaceholderText(QStringLiteral("����"));
    pPasswordLineEdit->setEchoMode(QLineEdit::Password);
    pRememberCheckBox->setText(QStringLiteral("��ס����"));
   /* pAutoLoginCheckBox->setText(QStringLiteral("�Զ���¼"));*/
    pLoginButton->setText(QStringLiteral("��¼"));
    pRegisterLabel->setText(QStringLiteral("���û�ע��"));
    pForgotLabel->setText(QStringLiteral("��������?"));

    QGridLayout* pLayout = new QGridLayout();
    // widget name ��1�У���0�п�ʼ��ռ2��2��
    pLayout->addWidget(widgetNameLabel, 1, 0, 2, 2);
    // �û�������� ��3�У���0�п�ʼ��ռ1��3��
    pLayout->addWidget(pUserLineEdit, 3, 0, 1, 3);
    /*pLayout->addWidget(pRegisterLabel, 5, 0, 1, 2);*/
    // ��������� ��1�У���1�п�ʼ��ռ1��2��
    pLayout->addWidget(pPasswordLineEdit, 5, 0, 1, 2);
    pLayout->addWidget(pForgotLabel, 5, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    // ��¼��ť ��3�У���1�п�ʼ��ռ1��2��
    pLayout->addWidget(pLoginButton, 7, 0, 1, 3);

    // ��ס���� ��2�У���1�п�ʼ��ռ1��1�� ˮƽ���� ��ֱ����
    pLayout->addWidget(pRememberCheckBox, 8, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    pLayout->addWidget(pRegisterLabel, 8, 0, 1, 3, Qt::AlignLeft | Qt::AlignVCenter);
    //// �Զ���¼ ��2�У���2�п�ʼ��ռ1��1�� ˮƽ���� ��ֱ����
    //pLayout->addWidget(pAutoLoginCheckBox, 2, 2, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    // ����ˮƽ���
    pLayout->setHorizontalSpacing(10);
    // ���ô�ֱ���
    pLayout->setVerticalSpacing(10);
    // ��������
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
