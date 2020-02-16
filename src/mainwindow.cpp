#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , videoReceiver(new VideoReceiver(this))
    , currentVehicle(1)
    , vehicle_data(new AS::Vehicle_Data_t)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/assets/icons/main.svg"));
    QCoreApplication::setOrganizationName("Teleportal");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("Teleportal");

    manual_control.x = 0;
    manual_control.y = 0;
    manual_control.z = 500;
    manual_control.r = 0;
    manual_control.buttons = 0;

    pressedKey.W = false;
    pressedKey.S = false;
    pressedKey.A = false;
    pressedKey.D = false;
    pressedKey.Up = false;
    pressedKey.Down = false;
    pressedKey.Left = false;
    pressedKey.Down = false;

    keyControlValue.forward = 400;
    keyControlValue.backward = -400;
    keyControlValue.leftward = -400;
    keyControlValue.rightward = 400;
    keyControlValue.upward = 700;
    keyControlValue.downward = 300;
    keyControlValue.turnLeft = -400;
    keyControlValue.turnRight = 400;

    on_actionVideo_triggered();

    setupToolBars();

    std::string ip("192.168.2.");
    AS::as_api_init(ip.c_str(), F_THREAD_NAMED_VAL_FLOAT | F_STORAGE_NONE);

    setupTimer();

    videoReceiver->start(ui->quickWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupToolBars()
{
    QList<QAction *> actionListDisarm;
    // actionListDisarm.append(ui->actionDisarm);
    ui->vehicleToolBar->addActions(actionListDisarm);

    armCheckBox = new QCheckBox(this);
    armCheckBox->setText("Arm");
    ui->vehicleToolBar->addWidget(armCheckBox);
    QObject::connect(armCheckBox, &QCheckBox::stateChanged,
                     this, &MainWindow::armCheckBox_stateChanged);

    ui->vehicleToolBar->addSeparator();

    QLabel *modeLable = new QLabel(this);
    modeLable->setText("Mode: ");
    ui->vehicleToolBar->addWidget(modeLable);

    modeComboBox = new QComboBox(this);
    modeComboBox->addItem("Manual");
    modeComboBox->addItem("Stabilize");
    modeComboBox->addItem("Depth Hold");
    ui->vehicleToolBar->addWidget(modeComboBox);
    QObject::connect(modeComboBox, 
                     static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     this, 
                     &MainWindow::modeComboBox_currentIndexChanged);

    // status toolbar
    QLabel *yawLabel = new QLabel("Yaw: ", this);
    yawLabelValue = new QLabel("0.00", this);
    QLabel *pitchLabel = new QLabel("Pitch: ", this);
    pitchLabelValue = new QLabel("0.00", this);
    QLabel *rollLabel = new QLabel("Roll: ", this);
    rollLabelValue = new QLabel("0.00", this);
    QLabel *depthLabel = new QLabel("Depth: ", this);
    depthLabelValue = new QLabel("0.00", this);

    yawLabelValue->setFixedWidth(50);
    pitchLabelValue->setFixedWidth(50);
    rollLabelValue->setFixedWidth(50);
    depthLabelValue->setFixedWidth(50);

    
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->statusToolBar->addWidget(spacer);

    ui->statusToolBar->addWidget(yawLabel);
    ui->statusToolBar->addWidget(yawLabelValue);
    ui->statusToolBar->addWidget(pitchLabel);
    ui->statusToolBar->addWidget(pitchLabelValue);
    ui->statusToolBar->addWidget(rollLabel);
    ui->statusToolBar->addWidget(rollLabelValue);
    ui->statusToolBar->addWidget(depthLabel);
    ui->statusToolBar->addWidget(depthLabelValue);

    QLabel *bannerLabel = new QLabel(this);
    bannerLabel->setFixedWidth(145);
    bannerLabel->setFixedHeight(15);
    bannerLabel->setStyleSheet("border-image: url(:/assets/logo/Logo-Large.png);");
    ui->statusToolBar->addWidget(bannerLabel);

    ui->menuPage->setStyleSheet("border-image: url(:/assets/keyboardControls.png);");
}

void MainWindow::setupTimer()
{
    QObject::connect(&vehicleDataUpdateTimer, &QTimer::timeout, this, &MainWindow::updateVehicleData);
    vehicleDataUpdateTimer.setInterval(25);
    vehicleDataUpdateTimer.start();

    QObject::connect(&manualControlTimer, &QTimer::timeout, this, &MainWindow::manualControl);
    manualControlTimer.setInterval(50);
    manualControlTimer.start();
}

void MainWindow::updateVehicleData()
{
    if (!AS::as_api_check_vehicle(currentVehicle))
    {
        // qDebug() << "vehicle: " << currentVehicle << "is not ready!";
        return;
    }

    AS::as_api_get_vehicle_data2(currentVehicle, vehicle_data);

    if (vehicle_data == nullptr)
    {
        // qDebug() << "no vehicle data!";
        return;
    }

    float yaw = 0, roll = 0, pitch = 0, depth = 0;

    const float degreePerRad = 180.0f / 3.1415926f;
    yaw = vehicle_data->yaw * degreePerRad;
    roll = vehicle_data->roll * degreePerRad;
    pitch = vehicle_data->pitch * degreePerRad;
    depth = vehicle_data->alt / 1000.0f;

    ui->qADI->setData(roll, pitch);
    ui->qCompass->setYaw(yaw);
    ui->qCompass->setAlt(depth);

    rollLabelValue->setNum(round(roll * 100) / 100.0);
    pitchLabelValue->setNum(round(pitch * 100) / 100.0);
    yawLabelValue->setNum(round(yaw * 100) / 100.0);
    depthLabelValue->setNum(round(depth * 100) / 100.0);
}

void MainWindow::manualControl()
{
    if (armCheckBox->checkState() == Qt::Checked &&
        AS::as_api_check_vehicle(currentVehicle))
    {
        AS::as_api_manual_control(manual_control.x, manual_control.y,
                                  manual_control.z, manual_control.r,
                                  manual_control.buttons, currentVehicle);

        // qDebug() << "manual control:" << manual_control.x << manual_control.y << manual_control.z << manual_control.r;
        manual_control.buttons = 0;
    }
}

void MainWindow::resizeWindowsManual()
{
    int m_width = ui->videoPage->width();

    ui->quickWidget->setGeometry(0, 0, m_width, ui->videoPage->height());

    ui->qCompass->setGeometry(m_width - 160, 0, 160, 160);
    ui->qADI->setGeometry(m_width - 320, 0, 160, 160);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeWindowsManual();

    QMainWindow::resizeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        return;
    }

    if (event->key() == Qt::Key_W)
    {
        qDebug() << "You Pressed Key W";
        pressedKey.W = true;
        manual_control.z = keyControlValue.upward;
    }
    else if (event->key() == Qt::Key_S)
    {
        qDebug() << "You Pressed Key S";
        pressedKey.S = true;
        manual_control.z = keyControlValue.downward;
    }
    else if (event->key() == Qt::Key_A)
    {
        qDebug() << "You Pressed Key A";
        pressedKey.A = true;
        manual_control.r = keyControlValue.turnLeft;
    }
    else if (event->key() == Qt::Key_D)
    {
        qDebug() << "You Pressed Key D";
        pressedKey.D = true;
        manual_control.r = keyControlValue.turnRight;
    }
    else if (event->key() == Qt::Key_Up)
    {
        qDebug() << "You Pressed Key Up";
        pressedKey.Up = true;
        manual_control.x = keyControlValue.forward;
    }
    else if (event->key() == Qt::Key_Down)
    {
        qDebug() << "You Pressed Key Down";
        pressedKey.Down = true;
        manual_control.x = keyControlValue.backward;
    }
    else if (event->key() == Qt::Key_Left)
    {
        qDebug() << "You Pressed Key Left";
        pressedKey.Left = true;
        manual_control.y = keyControlValue.leftward;
    }
    else if (event->key() == Qt::Key_Right)
    {
        qDebug() << "You Pressed Key Right";
        pressedKey.Right = true;
        manual_control.y = keyControlValue.rightward;
    }
    // else if (event->key() == Qt::Key_R)
    // {
    //     qDebug() << "You Pressed Key R";
    //     manual_control.buttons = 22;
    // }
    // else if (event->key() == Qt::Key_F)
    // {
    //     qDebug() << "You Pressed Key F";
    //     manual_control.buttons = 23;
    // }
    else
    {
        qDebug() << "You Pressed NOT supported Key";
    }

    qDebug() << "x: " << manual_control.x;
    qDebug() << "y: " << manual_control.y;
    qDebug() << "z: " << manual_control.z;
    qDebug() << "r: " << manual_control.r;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        return;
    }

    if (event->key() == Qt::Key_W)
    {
        qDebug() << "You Released Key W";
        if (pressedKey.S)
        {
            manual_control.z = keyControlValue.downward;
        }
        else
        {
            manual_control.z = 500;
        }
        pressedKey.W = false;
    }
    else if (event->key() == Qt::Key_S)
    {
        qDebug() << "You Released Key S";
        if (pressedKey.W)
        {
            manual_control.z = keyControlValue.upward;
        }
        else
        {
            manual_control.z = 500;
        }
        pressedKey.S = false;
    }
    else if (event->key() == Qt::Key_A)
    {
        qDebug() << "You Released Key A";
        if (pressedKey.D)
        {
            manual_control.r = keyControlValue.turnRight;
        }
        else
        {
            manual_control.r = 0;
        }
        pressedKey.A = false;
    }
    else if (event->key() == Qt::Key_D)
    {
        qDebug() << "You Released Key D";
        if (pressedKey.A)
        {
            manual_control.r = keyControlValue.turnLeft;
        }
        else
        {
            manual_control.r = 0;
        }
        pressedKey.D = false;
    }
    else if (event->key() == Qt::Key_Up)
    {
        qDebug() << "You Released Key Up";
        if (pressedKey.Down)
        {
            manual_control.x = keyControlValue.backward;
        }
        else
        {
            manual_control.x = 0;
        }
        pressedKey.Up = false;
    }
    else if (event->key() == Qt::Key_Down)
    {
        qDebug() << "You Released Key Down";
        if (pressedKey.Up)
        {
            manual_control.x = keyControlValue.forward;
        }
        else
        {
            manual_control.x = 0;
        }
        pressedKey.Down = false;
    }
    else if (event->key() == Qt::Key_Left)
    {
        qDebug() << "You Released Key Left";
        if (pressedKey.Right)
        {
            manual_control.y = keyControlValue.rightward;
        }
        else
        {
            manual_control.y = 0;
        }
        pressedKey.Left = false;
    }
    else if (event->key() == Qt::Key_Right)
    {
        qDebug() << "You Released Key Right";
        if (pressedKey.Left)
        {
            manual_control.y = keyControlValue.leftward;
        }
        else
        {
            manual_control.y = 0;
        }
        pressedKey.Right = false;
    }
    else
    {
        qDebug() << "You Released NOT supported Key";
    }

    qDebug() << "x: " << manual_control.x;
    qDebug() << "y: " << manual_control.y;
    qDebug() << "z: " << manual_control.z;
    qDebug() << "r: " << manual_control.r;
}

void MainWindow::on_actionVideo_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(0);
    ui->actionVideo->setChecked(true);
    ui->actionVideo->setDisabled(true);
    ui->actionMenu->setChecked(false);
    ui->actionMenu->setDisabled(false);
}

void MainWindow::on_actionMenu_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    ui->actionMenu->setChecked(true);
    ui->actionMenu->setDisabled(true);
    ui->actionVideo->setChecked(false);
    ui->actionVideo->setDisabled(false);
}

void MainWindow::modeComboBox_currentIndexChanged(int index)
{
    if (!AS::as_api_check_vehicle(currentVehicle))
    {
        qDebug() << "vehicle: " << currentVehicle << "is not ready!";
        return;
    }

    switch (index)
    {
    case 0:
        AS::as_api_set_mode(currentVehicle, AS::MANUAL);
        break;

    case 1:
        AS::as_api_set_mode(currentVehicle, AS::STABILIZE);
        break;

    case 2:
        AS::as_api_set_mode(currentVehicle, AS::ALT_HOLD);
        break;

    default:
        break;
    }
}

void MainWindow::armCheckBox_stateChanged(int state)
{
    if (!AS::as_api_check_vehicle(currentVehicle))
    {
        qDebug() << "vehicle: " << currentVehicle << "is not ready!";
        armCheckBox->setCheckState(Qt::Unchecked);
        return;
    }

    if (state == Qt::Checked)
    {
        AS::as_api_vehicle_arm(currentVehicle, 1);
        qDebug() << "ARM";
    }
    else if (state == Qt::Unchecked)
    {
        // set manual control value to zero
        manual_control.x = 0;
        manual_control.y = 0;
        manual_control.z = 500;
        manual_control.r = 0;
        manual_control.buttons = 0;

        AS::as_api_vehicle_disarm(currentVehicle, 1);

        qDebug() << "DISARM";
    }
}
