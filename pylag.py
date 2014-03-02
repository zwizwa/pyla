from PySide import QtGui, QtCore
import sys
import pyla
import stream

class console(QtGui.QTextEdit):
    """Console output log.  Can be connected to sys.stdout and/or sys.stderr."""
    def __init__(self, parent, poll):
        super(console,self).__init__(parent)
        self.setReadOnly(1)
        self.setCurrentFont(QtGui.QFont("monospace")) # "monospace" doesn't work on XP -> user Courier?
        self.setFontPointSize(8)
        self._poll = poll
        self._stdout = sys.stdout

    def __getattr__(self, name):
        return getattr(self._stdout, name)

    def write(self, bs):
        self._stdout.write(bs)
        self.insertPlainText(bs)
        # self.ensureCursorVisible()
        self._poll()



class pylag(QtGui.QWidget):
    """Pyla GUI."""
    def __init__(self, app):
        super(pylag,self).__init__()
        self._app = app
        self.initUI()
        pyla.register_poll(self.poll)
        self._abort = False

    def initUI(self):

        # Replace stdout/stderr with console window.
        self._console = console(self,self.poll)
        sys.stdout = self._console
        sys.stderr = self._console

        # Panel / log splitter.
        splitter = QtGui.QSplitter(QtCore.Qt.Vertical)
        vbox = QtGui.QVBoxLayout(self)
        vbox.addWidget(splitter)
        self.setLayout(vbox)
        self._vbox = vbox


        # FIXME: break recursion.
        def analyzer(text):
            dumper = getattr(stream, text)
            def action():
                print("Starting %s" % text)
                try:
                    dumper(log=self._console)
                except Exception as e:
                    print(e)
            btn = QtGui.QPushButton(text,self)
            btn.clicked.connect(action)
            splitter.addWidget(btn)
            return btn

        analyzer("dump_uart")
        analyzer("dump_syncser")

        splitter.addWidget(self._console)

        # Keyboard shortcuts
        start = QtGui.QShortcut(QtGui.QKeySequence(self.tr("Ctrl+A", "Start")), self)
        start.activated.connect(self.start)

        self.show()


    # TOOLS

    def poll(self):
        self._app.processEvents() # \update display


    # ACTIONS

    def start(self):
        pass

    def stop(self):
        pass

        


def main():
    app = QtGui.QApplication(sys.argv)
    ui = pylag(app=app)
    sys.exit(app.exec_())


# HACK: Make terminal CTRL-C work while in Qt mainloop.
# This does not do any cleanup!
import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)

if __name__ == '__main__':
    main()
