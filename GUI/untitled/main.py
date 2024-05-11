import sys
import os
from PySide6.QtWidgets import QApplication, QMainWindow, QTreeView, QFileSystemModel, QHBoxLayout, QWidget, QSplitter, QTextEdit, QPushButton, QMenu
from PySide6.QtCore import Qt, QDir
from PySide6.QtGui import QAction

class FileManager(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Файловый менеджер")
        self.setGeometry(100, 100, 800, 600)

        self.setup_ui()

    def setup_ui(self):
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)

        layout = QHBoxLayout(central_widget)

        # Создание модели файловой системы
        model = QFileSystemModel()
        model.setRootPath("")
        model.setFilter(QDir.AllDirs | QDir.Files | QDir.NoDotAndDotDot)  # Исключение папок "." и ".."

        # Создание виджета TreeView для левой панели
        tree_view = QTreeView()
        tree_view.setModel(model)
        tree_view.setRootIndex(model.index(os.path.expanduser("~/backup/projects/")))  # Установка корневого каталога

        tree_view.setHeaderHidden(True)

        # Удаление столбцов, кроме столбца "Name"
        for column in range(model.columnCount()):
            header = model.headerData(column, Qt.Horizontal)
            if header != "Name":
                tree_view.hideColumn(column)

        tree_view.expanded.connect(self.handle_item_expanded)  # Подключение сигнала expanded к обработчику
        tree_view.doubleClicked.connect(self.handle_item_double_clicked)  # Подключение сигнала doubleClicked к обработчику
        tree_view.clicked.connect(self.handle_item_clicked)
        tree_view.setContextMenuPolicy(Qt.CustomContextMenu)  # Установка политики контекстного меню

        tree_view.customContextMenuRequested.connect(self.show_context_menu)  # Подключение сигнала customContextMenuRequested к обработчику

        # Создание виджета QTextEdit для отображения содержимого файла
        file_text_edit = QTextEdit()
        file_text_edit.setReadOnly(True)

        # Создание виджета TreeView для правой панели
        right_tree_view = QTreeView()
        right_tree_view.setModel(model)
        right_tree_view.setHeaderHidden(True)

        right_tree_view.doubleClicked.connect(self.handle_right_item_double_clicked)  # Подключение сигнала doubleClicked к обработчику

        # Создание кнопки "Назад"
        self.back_button = QPushButton("Назад")
        self.back_button.clicked.connect(self.handle_back_button_clicked)
        self.back_button.hide()  # Скрытие кнопки изначально

        # Создание Splitter для разделения левой и правой панелей
        splitter = QSplitter()
        splitter.addWidget(tree_view)
        splitter.addWidget(right_tree_view)

        layout.addWidget(splitter)
        layout.addWidget(file_text_edit)
        layout.addWidget(self.back_button)

        self.tree_view = tree_view
        self.right_tree_view = right_tree_view
        self.file_text_edit = file_text_edit
        self.model = model

    def handle_item_clicked(self, index):
        if self.model.isDir(index):  # Проверка, является ли элемент папкой
            file_path = self.model.filePath(index)  # Получение пути к папке
            file_path = file_path.replace("/projects/", "/settings/") + ".settings"  # Замена части пути и добавление расширения
            print(file_path)
            self.display_file_content(file_path)

    def handle_item_expanded(self, index):
        # При раскрытии папки сворачиваем ее обратно
        self.tree_view.setExpanded(index, False)

    def handle_item_double_clicked(self, index):
        if self.model.isDir(index):  # Проверка, является ли элемент папкой
            folder_path = self.model.filePath(index)  # Получение пути к папке
            self.display_folder_contents(folder_path)
        else:
            self.back_button.show()  # Показкнопки "Назад" при открытии файла

    def handle_right_item_double_clicked(self, index):
        if not self.model.isDir(index):  # Проверка, является ли элемент файлом
            file_path = self.model.filePath(index)  # Получение пути к файлу
            self.display_file_content(file_path)

    def display_folder_contents(self, folder_path):
        # Установка корневого каталога для правого TreeView
        self.right_tree_view.setRootIndex(self.model.index(folder_path))

    def display_file_content(self, file_path):
        # Отображение содержимого файла в QTextEdit
        with open(file_path, "r") as file:
            content = file.read()
            self.file_text_edit.setPlainText(content)

        # Замена правой панели с QTreeView на QTextEdit
        layout = self.centralWidget().layout()
        layout.removeWidget(self.right_tree_view)
        self.right_tree_view.setParent(None)
        layout.addWidget(self.file_text_edit)
        self.back_button.show()  # Показ кнопки "Назад" при открытии файла

    def handle_back_button_clicked(self):
        self.file_text_edit.clear()  # Очистка содержимого QTextEdit
        self.back_button.hide()  # Скрытие кнопки "Назад"
        # Возвращение к представлению дерева файлов
        layout = self.centralWidget().layout()
        layout.removeWidget(self.file_text_edit)
        self.file_text_edit.setParent(None)
        layout.addWidget(self.right_tree_view)

    def show_context_menu(self, position):
        index = self.tree_view.indexAt(position)
        if index.isValid() and self.model.isDir(index):
            menu = QMenu(self.tree_view)
            commit_action = QAction("commit", self.tree_view)
            pull_action = QAction("pull", self.tree_view)
            remove_action = QAction("remove", self.tree_view)

            menu.addAction(commit_action)
            menu.addAction(pull_action)
            menu.addAction(remove_action)
            action = menu.exec_(self.tree_view.viewport().mapToGlobal(position))

            if action == commit_action:
                self.commit_project(index)
            elif action == pull_action:
                folder_path = self.model.filePath(index)
                self.pull_project(folder_path)
            elif action == remove_action:
                self.remove_action()

    def delete_folder(self, folder_path):
        print(folder_path)


    def run(self):
        self.show()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    file_manager = FileManager()
    file_manager.run()
    sys.exit(app.exec())
