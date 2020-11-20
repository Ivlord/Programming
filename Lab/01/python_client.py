#
#
#
#
#
#
#
#
#
#
# Всё в жуткой спешке. Никак не оптимизировалось.
# русский язык: даже не пробовал разбираться
#
import requests
import tkinter as tk
from tkinter.ttk import *
import json


def GetDataFromServer(srv='http://localhost:1234/raw', params=''):
    req = requests.get('http://localhost:1234/raw')

    if req.status_code!=200:
        print('Ошибка!')
        return {"city_name":"Error", "description":"access failed", "temp_int":"-" }
    print("ok")
    return req.json()

data=GetDataFromServer()

def reload_data(r):
    data=GetDataFromServer()


# Create the master object
main = tk.Tk()
main.geometry("120x150+250+400")
main.title("Weather forecast")
main.resizable(0, 0)
main.overrideredirect(1)

#stl = Style()
#stl.configure('pogoda', background='orange')

upframe = tk.Frame(master=main, bg='orange', width=0, height=0)
ceframe = tk.Frame(master=main, bg='white')
dnframe = tk.Frame(master=main, bg='orange')


#side=tk.LEFT, padx=10, pady=10
upframe.pack(fill="both", padx=0, pady=0 ) #, fill="both", expand=True
ceframe.pack(fill="both", expand=True)
dnframe.pack(fill="both", expand=True)

tk.Label(master=upframe, text=data["city_name"], bg='orange', font=("Helvetica", 16) ).pack()#.grid(row=0, column=0)
tk.Label(master=upframe, text=data["description"], bg='orange',font=("Helvetica", 8)).pack()#.grid(row=1, column=0)
tk.Label(master=ceframe, text=str(data["temp_int"])+"°C", bg='white',font=("Helvetica", 40)).pack()#.grid(row=1, column=0)

main.bind("<Button-1>", reload_data)

tk.mainloop()







'''
with open("in.json", "r") as read_file:
    data = json.load(read_file)
out=[]
for i in data:
    if not out or out[-1]['userId']!=i['userId']:
        out.append({'task_completed':0, 'userId':i['userId']})
    if i['completed']: out[-1]['task_completed']+=1
'''
