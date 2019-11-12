from kivy.app import App
from kivy.uix.boxlayout import BoxLayout

text = 'aaaa'

class Ttt(BoxLayout):
    def pp(self):
        print(text)

class TmpApp(App):
    def __init__(self, **kwargs):
        super(TmpApp, self).__init__(**kwargs)

    def build(self):
        t = Ttt()
        t.pp()
        return t

if __name__ == '__main__':
    TmpApp().run()