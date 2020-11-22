
import requests
import json
from tkinter import *

def GetNewData(r):
    err = False
    data = {'Town':' Ошибка', 'Desc': 'ошибка подключения', 'Temp':"??°C", 'Icon':'unknown'}
    try:
        desc.config(fg = "red")
        root.update_idletasks()
        result = requests.get('http://localhost:1234/raw')
    except:
        desc.config(fg = "black")
        root.update_idletasks()
        print('Ошибка получения данных.')
        err = True
    else:
        desc.config(fg = "black")
        root.update_idletasks()
        if result.status_code!=200:
            print('Код операции: ', result.status_code)
            data['Desc']='Ошибка' + str(req.status_code)
            err = True
        else:
            ret = result.json()
            if "temp" not in ret:
                err = True
    if not err:
        UpdateWidget( { 'Town':'Симферополь', 'Temp':str(int(ret["temp"])) + "°C",
                        'Desc': ret["description"].encode('l1').decode(),'Icon':ret["icon"]})
    else: UpdateWidget(data)

def UpdateWidget(data):
    town["text"] = data['Town']
    desc["text"] = data['Desc']
    temp["text"] = data['Temp']

    canvas.delete("all")
    my_img = PhotoImage(file = ".\\icons_for_python_client\\" + data['Icon'] + ".png")
    canvas.create_image(0, -10, anchor = 'nw', image = my_img)
    canvas.image = my_img

root = Tk()
root.geometry("180x270+300+400")
root.resizable(0, 0)
root.overrideredirect(1)

town   = Label(root, height=1, width=20, fg='black', bg='orange', font=("Times", 14, 'bold'))
desc   = Label(root, height=1, width=30, fg='black', bg='orange', font=("Times",  9, 'bold'))
temp   = Label(root, height=1, width=10, fg='black', bg='white' , font=("Times", 48, 'bold'))
footer = Label(root, height=2, width=30, bg="orange")
canvas = Canvas(height = 150, width = 130)

town.pack(  side=TOP)
desc.pack(  side=TOP)
temp.pack(  side=TOP)
footer.pack(side=BOTTOM)
canvas.pack(side=BOTTOM)

GetNewData(None)
root.bind("<Button-1>", GetNewData)
mainloop()
