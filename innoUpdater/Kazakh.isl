; *** Inno Setup version 5.1.11+ Russian messages ***
;
; Translation was made by Dmitry Kann, http://www.dk-soft.org/
; The highest accuracy was the first priority.
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Languages/Russian.isl,v 1.10 2007/02/28 04:38:21 jr Exp $

[LangOptions]
LanguageName=<0420><0443><0441><0441><043A><0438><0439>
LanguageID=$0419
LanguageCodePage=1251

[Messages]

; *** Application titles
SetupAppTitle=Обновление
SetupWindowTitle=Обновление — %1
UninstallAppTitle=Деинсталляция
UninstallAppFullTitle=Деинсталляция — %1

; *** Misc. common
InformationTitle=Информация
ConfirmTitle=Подтверждение
ErrorTitle=Ошибка

; *** SetupLdr messages
SetupLdrStartupMessage=Данная программа обновит %1 на Вашем компьютере, продолжить?
LdrCannotCreateTemp=Невозможно создать временный файл. Обновление прервано
LdrCannotExecTemp=Невозможно выполнить файл во временном каталоге. Обновление прервано

; *** Startup error messages
LastErrorMessage=%1.%n%nОшибка %2: %3
SetupFileMissing=Файл %1 отсутствует в папке обновления. Пожалуйста, устраните проблему или получите новую версию программы.
SetupFileCorrupt=Установочные файлы повреждены. Пожалуйста, получите новую копию программы.
SetupFileCorruptOrWrongVer=Эти установочные файлы повреждены или несовместимы с данной версией программы обновления. Пожалуйста, устраните проблему или получите новую копию программы.
NotOnThisPlatform=Эта программа не будет работать в %1.
OnlyOnThisPlatform=Эту программу можно запускать только в %1.
OnlyOnTheseArchitectures=Обновление этой программы возможна только в версиях Windows для следующих архитектур процессоров:%n%n%1
MissingWOW64APIs=В версии Windows, в которой Вы работаете, отсутствуют функции, необходимые для выполнения 64-битной обновления. Чтобы устранить эту проблему, Вам необходимо обновить пакет обновления (Service Pack) %1.
WinVersionTooLowError=Эта программа требует %1 версии %2 или выше.
WinVersionTooHighError=Программа не может быть обновлена в %1 версии %2 или выше.
AdminPrivilegesRequired=Чтобы обновить данную программу, Вы должны выполнить вход в систему как Администратор.
PowerUserPrivilegesRequired=Чтобы обновить эту программу, Вы должны выполнить вход в систему как Администратор или член группы «Опытные пользователи» (Power Users).
SetupAppRunningError=Обнаружен запущенный экземпляр %1.%n%nПожалуйста, закройте все экземпляры приложения, затем нажмите «OK», чтобы продолжить, или «Отмена», чтобы выйти.
UninstallAppRunningError=Деинсталлятор обнаружил запущенный экземпляр %1.%n%nПожалуйста, закройте все экземпляры приложения, затем нажмите «OK», чтобы продолжить, или «Отмена», чтобы выйти.

; *** Misc. errors
ErrorCreatingDir=Невозможно создать папку "%1"
ErrorTooManyFilesInDir=Невозможно создать файл в каталоге "%1", так как в нём слишком много файлов

; *** Setup common messages
ExitSetupTitle=Выход из программы обновления
ExitSetupMessage=Обновление не завершено. Если Вы выйдете, программа не будет обновлена.%n%nВы сможете завершить установку, запустив программу обновления позже.%n%nВыйти из программы обновления?
AboutSetupMenuItem=&О программе...
AboutSetupTitle=О программе
AboutSetupMessage=%1, версия %2%n%3%n%nСайт %1:%n%4
AboutSetupNote=
TranslatorNote=Russian translation by Dmitry Kann, http://www.dk-soft.org/

; *** Buttons
ButtonBack=< &Назад
ButtonNext=&Далее >
ButtonInstall=&Обновить
ButtonOK=OK
ButtonCancel=Отмена
ButtonYes=&Да
ButtonYesToAll=Да для &Всех
ButtonNo=&Нет
ButtonNoToAll=Н&ет для Всех
ButtonFinish=&Завершить
ButtonBrowse=&Обзор...
ButtonWizardBrowse=&Обзор...
ButtonNewFolder=&Создать папку

; *** "Select Language" dialog messages
SelectLanguageTitle=Выберите язык обновления
SelectLanguageLabel=Выберите язык, который будет использован в  процессе обновления:

; *** Common wizard text
ClickNext=Нажмите «Далее», чтобы продолжить, или «Отмена», чтобы выйти из программы обновления.
BeveledLabel=
BrowseDialogTitle=Обзор папок
BrowseDialogLabel=Выберите папку из списка и нажмите «ОК».
NewFolderName=Новая папка

; *** "Welcome" wizard page
WelcomeLabel1=Вас приветствует Мастер обновления [name]
WelcomeLabel2=Программа обновит [name/ver] на Ваш компьютер.%n%nРекомендуется закрыть все прочие приложения перед тем, как продолжить.

; *** "Password" wizard page
WizardPassword=Пароль
PasswordLabel1=Эта программа защищена паролем.
PasswordLabel3=Пожалуйста, наберите пароль, потом нажмите «Далее». Пароли необходимо вводить с учётом регистра.
PasswordEditLabel=&Пароль:
IncorrectPassword=Введенный Вами пароль неверен. Пожалуйста, попробуйте снова.

; *** "License Agreement" wizard page
WizardLicense=Лицензионное Соглашение
LicenseLabel=Пожалуйста, прочтите следующую важную информацию перед тем, как продолжить.
LicenseLabel3=Пожалуйста, прочтите следующее Лицензионное Соглашение. Вы должны принять условия этого соглашения перед тем, как продолжить.
LicenseAccepted=Я &принимаю условия соглашения
LicenseNotAccepted=Я &не принимаю условия соглашения

; *** "Information" wizard pages
WizardInfoBefore=Информация
InfoBeforeLabel=Пожалуйста, прочитайте следующую важную информацию перед тем, как продолжить.
InfoBeforeClickLabel=Когда Вы будете готовы продолжить установку, нажмите «Далее».
WizardInfoAfter=Информация
InfoAfterLabel=Пожалуйста прочитайте следующую важную информацию перед тем, как продолжить.
InfoAfterClickLabel=Когда Вы будете готовы продолжить установку, нажмите «Далее».

; *** "User Information" wizard page
WizardUserInfo=Информация о пользователе
UserInfoDesc=Пожалуйста, введите данные о себе.
UserInfoName=&Имя и фамилия пользователя:
UserInfoOrg=&Организация:
UserInfoSerial=&Серийный номер:
UserInfoNameRequired=Вы должны ввести имя.

; *** "Select Destination Location" wizard page
WizardSelectDir=Выбор папки обновления
SelectDirDesc=В какую папку Вы хотите обновить [name]?
SelectDirLabel3=Программа обновит [name] в следующую папку.
SelectDirBrowseLabel=Нажмите «Далее», чтобы продолжить. Если Вы хотите выбрать другую папку, нажмите «Обзор».
DiskSpaceMBLabel=Требуется как минимум [mb] Мб свободного дискового пространства.
ToUNCPathname=Обновление не может выполняться в папку по её сетевому имени. Если Вы устанавливаете в сетевую папку, Вы должны подключить её в виде сетевого диска.
InvalidPath=Вы должны указать полный путь с буквой диска; например:%n%nC:\APP%n%nили в форме UNC:%n%n\\имя_сервера\имя_ресурса
InvalidDrive=Выбранный Вами диск или сетевой путь не существуют или недоступны. Пожалуйста, выберите другой.
DiskSpaceWarningTitle=Недостаточно места на диске
DiskSpaceWarning=Обновление требует не менее %1 Кб свободного места, а на выбранном Вами диске доступно только %2 Кб.%n%nВы желаете тем не менее продолжить установку?
DirNameTooLong=Имя папки или путь к ней превышают допустимую длину.
InvalidDirName=Указанное имя папки недопустимо.
BadDirName32=Имя папки не может содержать символов: %n%n%1
DirExistsTitle=Папка существует
DirExists=Папка%n%n%1%n%nуже существует. Всё равно обновить в эту папку?
DirDoesntExistTitle=Папка не существует
DirDoesntExist=Папка%n%n%1%n%nне существует. Вы хотите создать её?

; *** "Select Components" wizard page
WizardSelectComponents=Выбор компонентов
SelectComponentsDesc=Какие компоненты должны быть установлены?
SelectComponentsLabel2=Выберите компоненты, которые Вы хотите обновить; снимите флажки с компонентов, устанавливать которые не требуется. Нажмите «Далее», когда Вы будете готовы продолжить.
FullInstallation=Полная установка
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Компактная установка
CustomInstallation=Выборочная установка
NoUninstallWarningTitle=Установленные компоненты
NoUninstallWarning=Программа обновления обнаружила, что следующие компоненты уже установлены на Вашем компьютере:%n%n%1%n%nОтмена выбора этих компонент не удалит их.%n%nПродолжить?
ComponentSize1=%1 Кб
ComponentSize2=%1 Мб
ComponentsDiskSpaceMBLabel=Текущий выбор требует не менее [mb] Мб на диске.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Выберите дополнительные задачи
SelectTasksDesc=Какие дополнительные задачи необходимо выполнить?
SelectTasksLabel2=Выберите дополнительные задачи, которые должны выполниться при обновлению [name], после этого нажмите «Далее»:

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Выберите папку в меню «Пуск»
SelectStartMenuFolderDesc=Где программа обновления должна создать ярлыки?
SelectStartMenuFolderLabel3=Программа создаст ярлыки в следующей папке меню «Пуск».
SelectStartMenuFolderBrowseLabel=Нажмите «Далее», чтобы продолжить. Если Вы хотите выбрать другую папку, нажмите «Обзор».
MustEnterGroupName=Вы должны ввести имя папки.
GroupNameTooLong=Имя папки группы или путь к ней превышают допустимую длину.
InvalidGroupName=Указанное имя папки недопустимо.
BadGroupName=Имя папки не может содержать символов:%n%n%1
NoProgramGroupCheck2=&Не создавать папку в меню «Пуск»

; *** "Ready to Install" wizard page
WizardReady=Всё готово к обновлению
ReadyLabel1=Программа обновления готова начать установку [name] на Ваш компьютер.
ReadyLabel2a=Нажмите «Обновить», чтобы продолжить, или «Назад», если Вы хотите просмотреть или изменить опции обновления.
ReadyLabel2b=Нажмите «Обновить», чтобы продолжить.
ReadyMemoUserInfo=Информация о пользователе:
ReadyMemoDir=Папка обновления:
ReadyMemoType=Тип обновления:
ReadyMemoComponents=Выбранные компоненты:
ReadyMemoGroup=Папка в меню «Пуск»:
ReadyMemoTasks=Дополнительные задачи:

; *** "Preparing to Install" wizard page
WizardPreparing=Подготовка к обновлению
PreparingDesc=Программа обновления подготавливается к обновлению [name] на Ваш компьютер.
PreviousInstallNotCompleted=Обновление или удаление предыдущей программы не были завершены. Вам потребуется перезагрузить Ваш компьютер, чтобы завершить ту установку.%n%nПосле перезагрузки запустите вновь Программу обновления, чтобы завершить установку [name].
CannotContinue=Невозможно продолжить установку. Нажмите «Отмена» для выхода из программы.

; *** "Installing" wizard page
WizardInstalling=Обновление...
InstallingLabel=Пожалуйста, подождите, пока [name] обновится на Ваш компьютер.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Завершение Мастера обновления [name]
FinishedLabelNoIcons=Программа [name] обновлена на Ваш компьютер.
FinishedLabel=Программа [name] обновлена на Ваш компьютер. Приложение можно запустить с помощью соответствующего значка.
ClickFinish=Нажмите «Завершить», чтобы выйти из программы обновления.
FinishedRestartLabel=Для завершения обновления [name] требуется перезагрузить компьютер. Произвести перезагрузку сейчас?
FinishedRestartMessage=Для завершения обновления [name] требуется перезагрузить компьютер.%n%nПроизвести перезагрузку сейчас?
ShowReadmeCheck=Я хочу просмотреть файл README
YesRadio=&Да, перезагрузить компьютер сейчас
NoRadio=&Нет, я произведу перезагрузку позже
; used for example as 'Run MyProg.exe'
RunEntryExec=Запустить %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Просмотреть %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Необходимо вставить следующий диск
SelectDiskLabel2=Пожалуйста, вставьте диск %1 и нажмите «OK».%n%nЕсли файлы этого диска могут быть найдены в папке, отличающейся от показанной ниже, введите правильный путь или нажмите «Обзор».
PathLabel=&Путь:
FileNotInDir2=Файл "%1" не найден в "%2". Пожалуйста, вставьте правильный диск или выберите другую папку.
SelectDirectoryLabel=Пожалуйста, укажите путь к следующему диску.

; *** Installation phase messages
SetupAborted=Обновление не была завершена.%n%nПожалуйста, устраните проблему и запустите установку снова.
EntryAbortRetryIgnore=Нажмите «Повтор», чтобы повторить попытку, «Пропустить», чтобы пропустить файл, или «Отказ» для отмены обновления.

; *** Installation status messages
StatusCreateDirs=Создание папок...
StatusExtractFiles=Распаковка файлов...
StatusCreateIcons=Создание ярлыков программы...
StatusCreateIniEntries=Создание INI-файлов...
StatusCreateRegistryEntries=Создание записей реестра...
StatusRegisterFiles=Регистрация файлов...
StatusSavingUninstall=Сохранение информации для деинсталляции...
StatusRunProgram=Завершение обновления...
StatusRollback=Откат изменений...

; *** Misc. errors
ErrorInternal2=Внутренняя ошибка: %1
ErrorFunctionFailedNoCode=%1: сбой
ErrorFunctionFailed=%1: сбой; код %2
ErrorFunctionFailedWithMessage=%1: сбой; код %2.%n%3
ErrorExecutingProgram=Невозможно выполнить файл:%n%1

; *** Registry errors
ErrorRegOpenKey=Ошибка открытия ключа реестра:%n%1\%2
ErrorRegCreateKey=Ошибка создания ключа реестра:%n%1\%2
ErrorRegWriteKey=Ошибка записи в ключ реестра:%n%1\%2

; *** INI errors
ErrorIniEntry=Ошибка создания записи в INI-файле "%1".

; *** File copying errors
FileAbortRetryIgnore=Нажмите «Повтор», чтобы повторить, «Пропустить», чтобы пропустить файл (не рекомендуется) или «Отказ» для выхода.
FileAbortRetryIgnore2=Нажмите «Повтор», чтобы повторить, «Пропустить», чтобы игнорировать ошибку (не рекомендуется) или «Отказ» для выхода.
SourceIsCorrupted=Исходный файл поврежден
SourceDoesntExist=Исходный файл "%1" не существует
ExistingFileReadOnly=Существующий файл помечен как «файл только для чтения».%n%nНажмите «Повтор» для удаления атрибута «только для чтения», «Пропустить», чтобы пропустить файл или «Отказ» для выхода.
ErrorReadingExistingDest=Произошла ошибка при попытке чтения существующего файла:
FileExists=Файл уже существует.%n%nПерезаписать его?
ExistingFileNewer=Существующий файл более новый, чем устанавливаемый. Рекомендуется сохранить существующий файл.%n%nВы хотите сохранить существующий файл?
ErrorChangingAttr=Произошла ошибка при попытке изменения атрибутов существующего файла:
ErrorCreatingTemp=Произошла ошибка при попытке создания файла в папке назначения:
ErrorReadingSource=Произошла ошибка при попытке чтения исходного файла:
ErrorCopying=Произошла ошибка при попытке копирования файла:
ErrorReplacingExistingFile=Произошла ошибка при попытке замены существующего файла:
ErrorRestartReplace=Ошибка RestartReplace:
ErrorRenamingTemp=Произошла ошибка при попытке переименования файла в папке назначения:
ErrorRegisterServer=Невозможно зарегистрировать DLL/OCX: %1
ErrorRegSvr32Failed=Ошибка при выполнении RegSvr32, код возврата %1
ErrorRegisterTypeLib=Невозможно зарегистрировать библиотеку типов (Type Library): %1

; *** Post-installation errors
ErrorOpeningReadme=Произошла ошибка при попытке открытия файла README.
ErrorRestartingComputer=Программе обновления не удалось перезапустить компьютер. Пожалуйста, выполните это самостоятельно.

; *** Uninstaller messages
UninstallNotFound=Файл "%1" не существует, деинсталляция невозможна.
UninstallOpenError=Невозможно открыть файл "%1". Деинсталляция невозможна
UninstallUnsupportedVer=Файл протокола для деинсталляции "%1" не распознан данной версией программы-деинсталлятора. Деинсталляция невозможна
UninstallUnknownEntry=Встретился неизвестный пункт (%1) в файле протокола для деинсталляции
ConfirmUninstall=Вы действительно хотите удалить %1 и все компоненты программы?
UninstallOnlyOnWin64=Данную программу возможно деинсталлировать только в среде 64-битной Windows.
OnlyAdminCanUninstall=Эта программа может быть деинсталлирована только пользователем с административными привилегиями.
UninstallStatusLabel=Пожалуйста, подождите, пока %1 будет удалена с Вашего компьютера.
UninstalledAll=Программа %1 была полностью удалена с Вашего компьютера.
UninstalledMost=Деинсталляция %1 завершена.%n%nЧасть элементов не удалось удалить. Вы можете удалить их самостоятельно.
UninstalledAndNeedsRestart=Для завершения деинсталляции %1 необходимо произвести перезагрузку Вашего компьютера.%n%nВыполнить перезагрузку сейчас?
UninstallDataCorrupted=Файл "%1" поврежден. Деинсталляция невозможна

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Удалить совместно используемый файл?
ConfirmDeleteSharedFile2=Система указывает, что следующий совместно используемый файл больше не используется никакими другими приложениями. Подтверждаете удаление файла?%n%nЕсли какие-либо программы всё еще используют этот файл, и он будет удалён, они не смогут работать правильно. Если Вы не уверены, выберите «Нет». Оставленный файл не навредит Вашей системе.
SharedFileNameLabel=Имя файла:
SharedFileLocationLabel=Расположение:
WizardUninstalling=Состояние деинсталляции
StatusUninstalling=Деинсталляция %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1, версия %2
AdditionalIcons=Дополнительные значки:
CreateDesktopIcon=Создать значок на &Рабочем столе
CreateQuickLaunchIcon=Создать значок в &Панели быстрого запуска
ProgramOnTheWeb=Сайт %1 в Интернете
UninstallProgram=Деинсталлировать %1
LaunchProgram=Запустить %1
AssocFileExtension=Св&язать %1 с файлами, имеющими расширение %2
AssocingFileExtension=Связывание %1 с файлами %2...
