#!/usr/bin/python3
from tkinter import *
import subprocess
import os

cur_chart, charts, a, b, C, cfg, x_offset = 0, [], [], [], None, [], 50

def element_bg_back(widg,kkey,val):
    widg[kkey]=val

def title_add(have_report=True):
    global C, cfg
    C.create_text(1000+x_offset,60+400, fill="blue",font="Times 16 bold", text="ПЗ-31.\nАнализ использования памяти\nпри изменении размеров вектора")
    C.create_text(1000+x_offset,100+400, fill="blue",font="Times 9 bold", text="Мазлов ИВТ-201-2")

    if have_report:
        txt_peredacha_type='ссылке' if cfg[0] else 'значению'
        txt_reserve = str(cfg[2]) + ' эл. при vect.reserve('+str(cfg[1])+')' if cfg[1] else str(cfg[2]) + ' эл. ' +'vect.reserve() не установлен'
        C.create_text(1090+x_offset,20+400, fill="red",font="Times 9 bold", text='Передача вектора по '+txt_peredacha_type+'\n'+txt_reserve)
    else:
        C.create_text(1090+x_offset,20+400, fill="red",font="Times 9 bold", text='Нет отчётов.\nСоздайте отчёт.')
    C.update()

def upload_chart_data(event, idx_add):
    global cur_chart, charts, a, b, C, cfg
    if not charts:
        chart_no['text']="no reports"
        return
    cur_chart+=idx_add
    if cur_chart<0:
        cur_chart=0
        return
    if cur_chart>=len(charts):
        cur_chart=len(charts)-1
        return

    chart_no['text']=charts[cur_chart]

    C.delete('all')
    with open(charts[cur_chart], 'r') as reader:
        a=eval (reader.read())

    cfg=a[0]
    a=a[1:]
    # получаем отсортированный массив встреченных адресов
    b=sorted(list(set(  {adr for rec in range(len(a)) for adr in a[rec]['adr']} ) ) )

    last_start_x = b[0] # начало адресов
    last_i=0
    # ищем сегменты памяти, проставляем линии
    segments=[] # [ [start, end, size]  ]

    y=555
    C.create_line(x_offset, 1, x_offset, 650, width=1, fill="red") #  0 4 82
    for i in range(len(b)-1):
        if (b[i]+4)!=b[i+1]: # делаем вертикальные линии блоков памяти
            y=555 if y>=615 else y+15
            C.create_line((i+1)*4-2+x_offset, 1, (i+1)*4-2+x_offset, 650, width=1, fill="green") #  0 4 82
            segments.append([last_start_x, b[i], (b[i]-last_start_x+1)//4+1])
            C.create_text(((i-last_i)//2)*4 + last_i*4+x_offset+2, y, fill="blue",font="Times 8 bold", text="<"+str(i-last_i+1) +">")
            last_start_x=b[i+1]
            last_i=i+1
    segments.append([last_start_x, b[-1], (b[-1]-last_start_x)//4+1])
    C.create_text(((len(b)-last_i)//2)*4 + last_i*4+x_offset+2, y, fill="blue",font="Times 8 bold", text="<"+str(len(b)-last_i) +">")
    C.create_line(len(b)*4-2+x_offset, 1, len(b)*4-2+x_offset, 650, width=1, fill="green")

    cap=1
    f=[]
    last_first_adr=a[0]['adr'][0]

    for y in range(len(a)):
        if cap!=a[y]['capacity'] and y!=0:
            f.append(str(a[y-1]['capacity']))
            if y<len(a)-1 and last_first_adr!=a[y]['adr'][0]:
                x = b.index(a[y-1]['adr'][0])
                txt, y1=" ".join(f), y
                if len(f)>15:
                    y1=y-6
                    txt=" ".join(f[:len(f)//2]) + '\n' + " ".join(f[len(f)//2:])
                C.create_text(x*4+x_offset, y1*2+0, fill="red",font="Times 8 bold", text=txt, anchor=NW)
                cap=a[y]['capacity']
                last_first_adr=a[y]['adr'][0]
                f=[]
        for adr in a[y]['adr']:
            x = b.index(adr)*4
            C.create_oval(x-1+x_offset, y*2+2 , x+2+x_offset, y*2+4, fill='blue' )

    if not a[-1]['adr']:
        x=b.index(a[-2]['adr'][0])
        cap=a[-2]['capacity']
    else:
        x=b.index(a[-1]['adr'][0])
        cap=a[-1]['capacity']

    C.create_text(x*4+x_offset, len(a)*2+0, text=str(cap), anchor=NW, fill="red",font="Times 8 bold")
    title_add()

def delete_report_file(*args):
    global cur_chart, charts
    if not charts: return
    os.remove(charts[cur_chart])
    del charts[cur_chart]
    if not charts:
        chart_no['text']="no reports"
        C.delete('all')
        title_add(False)
        return
    if cur_chart!=0:
        cur_chart-=1
    upload_chart_data(None, 0)

def rescan_reports(*args):
    global cur_chart, charts
    cur_file_name = charts[cur_chart] if charts else ""
    charts = [x for x in os.listdir() if x.endswith(".txt") and x[:4]=='data']
    if charts:
        cur_chart= charts.index(cur_file_name) if cur_file_name in charts else 0
        upload_chart_data(None, 0)
    else:
        chart_no['text']='no reports'

def gen_report(*args):
    global cur_chart, charts
    try:
        reserv_cfg=int(reserv_wiget.get())
        assert 0<=reserv_cfg<95,""
        reserv_wiget['bg']='green'
        root.after(300, element_bg_back, reserv_wiget, 'bg', 'white')
    except:
        reserv_wiget['bg']='red'
        root.after(300, element_bg_back, reserv_wiget, 'bg', 'white')
        reserv_wiget.delete(0,END) #.insert(0, g)
        return
    try:
        elem_cfg=int(elem_wiget.get())
        assert 1<elem_cfg<100,""
        elem_wiget['bg']='green'
        root.after(300, element_bg_back, elem_wiget, 'bg', 'white')
    except:
        elem_wiget['bg']='red'
        root.after(300, element_bg_back, elem_wiget, 'bg', 'white')
        elem_wiget.delete(0,END)
        return

    f = open('cfg.txt', 'w')
    f.write(str(elem_cfg)+'\n'+str(reserv_cfg))
    f.close()
    new_file_name="data_e"+str(elem_cfg)+"_res"+str(reserv_cfg)+("_lnk" if by_value_val.get() else "_val")+".txt"
    run_file='Zadanie_31_lnk.exe' if by_value_val.get() else 'Zadanie_31_val.exe'
    if not os.path.exists(run_file):
        print('Невозможно создать файл', new_file_name, ', отсутствует', run_file, '\nЧтобы скомпилировать его, следуйте инструкции в ReadMe.md')
        return
    subprocess.call(run_file)

    charts.append(new_file_name)
    cur_chart=len(charts)-1
    rescan_reports()
    return

root = Tk() ; root.geometry('1252x650+5+100'); root.title("Анализ памяти при изменении вектора");
C=Canvas(root, width='1250', height='650', bd=0) #, highlightthickness=0, relief='ridge'
C.place(x=0,y=0)

rem_file=Button(root, text="Delete report", font="Times 10", bd=1, fg='blue', padx=0, pady=0)
rem_file.bind("<Button-1>", delete_report_file)
rem_file.place(x=0,y=602)

bck=Button(root, text="<", font="Times 10", bd=1, fg='blue', padx=0, pady=0)
bck.bind("<Button-1>", lambda event: upload_chart_data(event, -1))
bck.place(x=0,y=625)

frw=Button(root, text=">", font="Times 10", bd=1, fg='blue', padx=0, pady=0)
frw.bind("<Button-1>", lambda event: upload_chart_data(event, 1))
frw.place(x=150,y=625)

chart_no=Label(root, text="", font="Times 10", bd=1, fg='blue', padx=0, pady=0)
chart_no.place(x=15,y=630)

Label(root, text="По ссылке  Элементов  Резерв", font=("Times", 10, 'bold'), fg='blue', padx=0, pady=0).place(x=900+40,y=605)

by_value_val = IntVar(value=1)
by_value_wiget=Checkbutton(root, text="", onvalue=1, offvalue=0, variable=by_value_val)
by_value_wiget.place(x=925+40,y=620)

elem_wiget = Entry(root, width=5)
elem_wiget.insert(10, 64)
elem_wiget.place(x=985+40, y=625)

reserv_wiget = Entry(root, width=5)
reserv_wiget.insert(10, 0)
reserv_wiget.place(x=1040+40, y=625)

frw=Button(root, text="Сгенерировать отчёт", font="Times 10", bd=1, fg='blue', padx=0, pady=0)
frw.bind("<Button-1>", gen_report)
frw.place(x=1080+40,y=622)

rescan_reports(None)
root.mainloop()
