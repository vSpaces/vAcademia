
class QAppMumble : public QApplication {
public:
	QUrl quLaunchURL;
	QAppMumble(int &pargc, char **pargv) : QApplication(pargc, pargv) {}
	void commitData(QSessionManager&);
	bool event(QEvent *e);
#ifdef Q_OS_WIN
	bool winEventFilter(MSG *msg, long *result);
#endif
};