# coding:utf-8

from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.button import Button

class MainScreen(BoxLayout):
    pass

class MainApp(App):
    def __init__(self, **kwargs):
        super(MainApp, self).__init__(**kwargs)

    def build(self):
        return MainScreen()

if __name__=="__main__":
    MainApp().run()
