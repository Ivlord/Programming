
import random
from tkinter import *
import numpy             as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk) # Implement the default Matplotlib key bindings.
from matplotlib.backend_bases          import key_press_handler
from matplotlib.figure                 import Figure
from matplotlib                        import style

class Bob:
#Bob([C, [ax0, ax0g, line0], [ax1, ax1g, line1]], [sc, e_L, e_g, e_dt, e_ang], (int(C['width'])/2, 195), start_speed=0, R=20, clr='blue')
    def __init__(self, Canvas, widg, origin, max_iter=30000, start_speed=0, R=20, clr='blue'):
        self.C, self.F, self.G = Canvas# канвасы для отрисовки: маятника, графика fi, гидографа
        self.widg        = widg #
        self.L, self.g =0,0
        self.algorithm   = 1            # verlet default
        self.alg_list    = [self.euler, self.verlet]
        self.origin      = origin       # Координата точки подвеса
        self.fi_0        = -100000      # rad: начальный угол отклонения фи
        self.max_iter    = max_iter-1   # Максимальное количество итераций
        self.start_speed = start_speed  # начальная угловая скорость
        self.mass = 1
        self.Lald =0
        self.gald =0
        self.R, self.clr, self.L_draw = R, clr, 165 # Радиус, цвет и длина линии(const) для отрисовки шара
        self.refresh_vars(*(float(w.get()) for w in self.widg)) #[0-sc-lambda, 1-e_L-Length, 2-e_g-gravity, 3-e_dt_timedt, 4-e_ang-нач.угол]
        self.upd(True)                                # X,Y текущая позиция шара на плоскости
    def refresh_vars(self, damp_decr, L, g, dt, ang):
        self.Lald, self.gald= self.L, self.g              # длина нити L и ускорение св. падения для маятника g
        self.L, self.g      = L, g                        # длина нити L и ускорение св. падения для маятника g
        self.damp_decr      = damp_decr                   # затухания
        self.damp_decr_x2   = 2*damp_decr                 # сразу вычисляем для формулы константу: двойной декремент затухания lambda: -sin(fi)*(g/L) - 2*lambda*spd
        self.minus_g_div_L  = -g/L                        # константа, используемая в вычислении ускорения с затуханием: -sin(fi)*(g/L) - 2*lambda*spd
        sqrt_L_div_g_2      = 2 * np.sqrt(L/g)            # промежуточная переменная: 2*pi*sqrt(L/g)
        self.T_Gugens       = np.pi * sqrt_L_div_g_2      # значение периода T = Формула Гюйгенса: 2*pi*sqrt(L/g)
        self.sqrt_L_div_g_4 = 2 * sqrt_L_div_g_2          # константа для формулы точного значения: 4*sqrt(L/g)*K(k)
        self.T_Gugens_div_16= self.T_Gugens/16            # константа для формулы уточненного значения формулы Гюйгенса: 2*pi*sqrt(L/g)(1+fi*fi/16)

        self.dt             = dt                          # шаг дискретизации времени
        self.dt0_5          = 0.5*dt                      # ещё кусок константы из формулы: шаг дискретизации времени / 2
        self.dt2            = self.dt0_5*dt               # кусок константы из формулы: шаг дискретизации времени в квадрате/2
        if ang!=0:
            print("full reset!", ang)
            ang = np.radians(ang)
            self.ticks      = 0             # тиков дискретизации (итераций)
            self.T          = 0             # высичленный период по проходу маятника (обновляется один раз в полупериод)
            self.Last_Half_T_time = 0       # последнее время прохожения точки равновейсия
            self.fi_0 = ang
            self.max_fi = np.absolute(self.fi_0)          # максимальное фи по итогам последнего T
            print("ang=", ang, ", max_fi=", self.max_fi, ", ticks=", self.ticks)
            self.ang, self.spd, self.acc, self.time, self.Ep, self.Ek = map(np.zeros, [self.max_iter] * 6)
            self.ang[0], self.spd[0], self.acc[0] = self.fi_0, self.start_speed, self.accel(self.start_speed, self.fi_0)
            self.grow    = self.fi_0 < 0    # используется для регистрации self.max_fi
            self.sections= 0
            self.Ep_raise=-1
            self.div = 50
            self.Emax = self.g * self.L * (1-np.cos(self.fi_0))
            self.x  = self.origin[0] + int(self.L_draw * np.sin(self.fi_0))   # X текущая позиция шара на плоскости
            self.y  = self.origin[1] + int(self.L_draw * np.cos(self.fi_0))   # Y
        if self.Lald!=self.L or self.gald!=self.g:
            self.Emax = g*L * (1-np.cos(self.fi_0))
            print("self.Emax=", self.Emax)
        self.update_Ts()
    def update_Ts(self):                                                # Обновить все значения периодов
        k   = 1 - np.sin(self.max_fi/2)**2
        k1  = (( (0.01451196212*k + 0.03742563713)*k + 0.03590092383)*k + 0.09666344259)*k + 1.38629436112
        k2  = (( (0.00441787012*k + 0.03328355346)*k + 0.06880248576)*k + 0.12498593597)*k + 0.5
        cei = k1 - k2 * np.log(k)                                               # полный элиптический интеграл
        self.T_K          = self.sqrt_L_div_g_4 * cei                           #
        self.T_GugensPlus = self.T_Gugens + self.T_Gugens_div_16*self.max_fi**2 # значение периода T = Уточненное значение
    def verlet(self, i=0):
        self.ang[i+1] = self.ang[i] + self.spd[i]*self.dt + self.acc[i]*self.dt2    # новый угол
        pr_spd        = self.spd[i] + self.acc[i]*self.dt                           # предикт скорости
        pr_acc        = self.accel(pr_spd, self.ang[i+1])                           # предикт ускорения
        self.spd[i+1] = self.spd[i] + self.dt0_5 * (self.acc[i]+pr_acc)             # новая угловая скорость
        self.acc[i+1] = self.accel(self.spd[i+1], self.ang[i+1])                    # новое ускорение
    def euler(self, i=0):                                                       # midpoint=0 - обычный метод Эйлера, midpoint=1 - медод Эйлера с усреднением
        self.ang[i+1] = self.ang[i] + self.dt * self.spd[i]                     # новый угол *self.Ep_raise
        self.spd[i+1] = self.spd[i] + self.acc[i]*self.dt                             # новая угловая скорость
        self.acc[i+1] = self.accel(self.spd[i+1], self.ang[i+1])                      # новое ускорение
    def accel(self, spd, ang):
        return np.sin(ang)*self.minus_g_div_L - self.damp_decr_x2*spd
    def upd(self, enforce=False):
        if self.ticks>=self.max_iter or np.absolute(self.max_fi)<0.0008726646259971648: # 0.05 degree
            C.create_oval(self.x-self.R, self.y-self.R, self.x+self.R, self.y+self.R, fill='red')
            C.create_text(self.x-95,self.y-35,fill="SteelBlue4",font="Times 14 italic bold", anchor='w', text="Итерации завершены")
            return False

        self.time[self.ticks+1] = self.time[self.ticks] + self.dt
        self.alg_list[self.algorithm](self.ticks)
        if self.ang[self.ticks+1]>2*np.pi: self.ang[self.ticks+1]-=2*np.pi
        if self.ang[self.ticks+1]<-2*np.pi: self.ang[self.ticks+1]+=2*np.pi

        self.Ep[self.ticks] = self.g * self.L * (1-np.cos(self.ang[self.ticks])) # mgh
        self.Ek[self.ticks] = 0.5 * (self.L*self.spd[self.ticks])**2             # 0.5mv^2
        Ecur = self.Ep[self.ticks] + self.Ek[self.ticks]

        self.Ep_raise=1 if self.Ep[self.ticks]>self.Ek[self.ticks-1] else -1


        self.nx  = self.origin[0] + int(self.L_draw * np.sin(self.ang[self.ticks])) # X текущая позиция шара на плоскости
        self.ny  = self.origin[1] + int(self.L_draw * np.cos(self.ang[self.ticks])) # Y

        self.ticks += 1   # [ticks]-current position, [ticks-1] previous(always exists)
        if enforce: self.ticks-=1
        if self.ticks>0:
            if self.grow and (self.ang[self.ticks-1]>self.ang[self.ticks]):
                self.grow    = False
                self.max_fi = self.ang[self.ticks-1]
            elif (not self.grow) and (self.ang[self.ticks-1]<self.ang[self.ticks]):
                self.grow    = True
                self.max_fi = -self.ang[self.ticks-1]

        # признак прохождения нижней точки: разные знаки у углов отклонения
        if np.sign(self.ang[self.ticks])!=np.sign(self.ang[self.ticks-1]):
            t0 = self.time[self.ticks] - self.dt*self.ang[self.ticks] /\
                                (self.ang[self.ticks] - self.ang[self.ticks-1])
            if self.Last_Half_T_time!=0:
                self.T = (t0 - self.Last_Half_T_time) * 2
            self.Last_Half_T_time = t0

        if not self.ticks%5:
            if self.time[self.ticks]//self.div > self.sections: # [ax0, ax0g, line0]
                self.sections+=1 #self.F[0].xaxis.set_major_formatter(ticker.FormatStrFormatter('%0.1f'))
                self.F[0].clear()
                self.F[2], = self.F[0].plot(self.time[:self.ticks], self.ang[:self.ticks], 'r-')
                #self.F[2], = self.F[0].plot(self.time[:self.ticks], self.Ek[:self.ticks], 'r-')
                self.div = min([5,10,20,50,100,200,500,1000,2000,5000], key=lambda x:abs(x-(self.div+self.sections*self.div)//5))
                self.F[0].xaxis.set_ticks(np.arange(0, self.div+self.sections*self.div+1, self.div))
                self.F[1].draw()
                #self.F[1].flush_events() #fig0.tight_layout()
            else:
                self.F[2].set_xdata(self.time[:self.ticks])
                self.F[2].set_ydata(self.ang[:self.ticks])
                #self.F[2].set_ydata(self.Ek[:self.ticks])
                self.F[1].draw()
                self.F[1].flush_events()  #self.G[0].plot(self.ang[:self.ticks], self.spd[:self.ticks])
            self.G[2].set_xdata(self.ang[:self.ticks])
            self.G[2].set_ydata(self.spd[:self.ticks])
            self.G[1].draw()
            self.G[1].flush_events()

        if not self.ticks%2:
            self.nx  = self.origin[0] + int(self.L_draw * np.sin(self.ang[self.ticks])) # X текущая позиция шара на плоскости
            self.ny  = self.origin[1] + int(self.L_draw * np.cos(self.ang[self.ticks])) # Y
            self.x, self.y = self.nx, self.ny
            C.delete("all")
            C.create_line(self.origin[0]-10, self.origin[1], self.origin[0]+10, self.origin[1], width=5, fill="green")
            C.create_line(self.origin[0], 390, self.origin[0], 380,                             width=5, fill="red")
            C.create_line(self.origin[0], self.origin[1], self.x, self.y, width=2, fill="green")  # нить
            C.create_text(self.origin[0],self.origin[0]-20,fill="SteelBlue4",font="Times 14 italic bold", text=str(round(np.degrees(self.ang[self.ticks]),1))+"°")
            C.create_text(10,20,fill="SteelBlue4",font="Times 10 italic bold", anchor='w', text="Общее время= "+str(round(self.time[self.ticks],1))+"с")
            C.create_text(10,35,fill="SteelBlue4",font="Times 10 italic bold", anchor='w', text="Итераций= "+str(self.ticks))
            C.create_text(310,20,fill="SteelBlue4",font="Times 10 italic bold", anchor='w', text="fi max= "+str(round(np.degrees(self.max_fi),1))+"°")

            C.create_oval(self.x-self.R, self.y-self.R, self.x+self.R, self.y+self.R, fill=self.clr)
            C.create_oval(self.x-self.R- 5, self.y-self.R- 5, self.x+self.R+ 5, self.y+self.R+ 5, outline='')
            C.create_oval(self.x-self.R-10, self.y-self.R-10, self.x+self.R+10, self.y+self.R+10, outline='')
            C.create_text(200,395,fill="SteelBlue4",font="Times 10 italic bold", text=str(round(self.time[self.ticks]-self.Last_Half_T_time, 2))+'с')

            C.create_rectangle(390, 180, 400, 380,                             outline='magenta',width=1, fill="")
            #print(Ecur, self.Emax, self.Ep[self.ticks],self.Ek[self.ticks], self.ticks, self.g)
            Yp = int(self.Ep[self.ticks-1]/Ecur*200*(Ecur/self.Emax))
            Yk = int(self.Ek[self.ticks-1]/Ecur*200*(Ecur/self.Emax))
            if Yp: C.create_rectangle(392, 380-Yp, 398, 380, outline='',width=1, fill="red")
            if Yk: C.create_rectangle(392, 380-Yp-Yk, 398, 380-Yp-1, outline='',width=1, fill="blue")


            C.update()
        return True
def m():
    global O, paused
    if paused:
        root.after(animate_speed, m)
        return
    if not O.upd():
        pause(True)
    else:
        O.update_Ts()
        Better_L["text"], Calcul_L["text"], Zasech_L["text"]=[str(round(i,10)) for i in [O.T_GugensPlus, O.T_K, O.T]]
    root.after(animate_speed, m)

def element_bg_back(widg,kkey,val):
    widg[kkey]=val
def change_dropdown_g(*args):
    global busy_mode
    if busy_mode or tkvar_g.get()=='Другой': return
    e_g.delete(0,END); e_g.insert(0, G_preset[tkvar_g.get()])
    use_entered_g()
def use_entered_g(*args):
    global busy_mode
    try:
        g=float(e_g.get())
        e_g['bg']='green'; busy_mode=True
        tkvar_g.set(([key for key in G_preset if G_preset[key]==g]+['Другой'])[0])
        busy_mode=False
        O.refresh_vars(O.damp_decr, O.L, g, O.dt, 0)
    except:
        e_g['bg']='red'; e_g.delete(0,END); e_g.insert(0, O.g)
    root.after(300, element_bg_back, e_g, 'bg', 'white')
def change_dropdown_dt(*args):
    global busy_mode
    if busy_mode or tkvar_dt.get()=='Другой': return
    e_dt.delete(0,END); e_dt.insert(0, dt_preset[tkvar_dt.get()])
    use_entered_dt()
def use_entered_dt(*args):
    global busy_mode
    try:
        dt=float(e_dt.get())
        e_dt['bg']='green'; busy_mode=True
        tkvar_dt.set(([key for key in dt_preset if dt_preset[key]==dt]+['Другой'])[0])
        busy_mode=False
        O.refresh_vars(O.damp_decr, O.L, O.g, dt, 0)
    except:
        e_dt['bg']='red'; e_dt.delete(0,END); e_dt.insert(0, O.dt)
    root.after(300, element_bg_back, e_dt, 'bg', 'white')
def use_entered_L(*args):
    try:
        L=float(e_L.get()); e_L['bg']='green'
        O.refresh_vars(O.damp_decr, L, O.g, O.dt, 0)
    except:
        e_L['bg']='red'; e_L.delete(0,END); e_L.insert(0, O.L)
    root.after(300, element_bg_back, e_L, 'bg', 'white')
def use_entered_ang(*args):
    try:
        ang=float(e_ang.get()); e_ang['bg']='green'
        if ang<-360 or ang>360:
            ang=(np.absolute(ang)%360)*np.sign(ang) #ang=((ang%360)+360)%360
            e_ang.delete(0,END); e_ang.insert(0, ang)
        if np.radians(ang)!=O.fi_0:
            O.refresh_vars(O.damp_decr, O.L, O.g, O.dt, ang)
            if not paused: pause(True)
            O.upd(True)
    except:
        e_ang['bg']='red'; e_ang.delete(0,END); e_ang.insert(0, np.degrees(O.fi_0))
    root.after(300, element_bg_back, e_ang, 'bg', 'white')
def change_dropdown_alg(*args):
    O.algorithm = alg_preset[tkvar_alg.get()] #O.(alg_preset[tkvar_alg.get()])()
def change_sc(*args):
    O.refresh_vars(float(sc.get()), O.L, O.g, O.dt, 0) # damp_decr
def pause(*args, enforce=[]):
    global paused
    if enforce!=[]: paused = enforce
    else: paused = not paused
    b_pause.configure(text=[" Остановить ", " Запустить "][paused])

paused = True
busy_mode=False
animate_speed=10
starting_angel=179
sar=np.radians(starting_angel)
menu_style = {"bd":0, "font":("Times", 10, 'bold'), "bg":'SteelBlue3', "fg":'black'}

root = Tk() ; root.geometry('%dx%d+%d+%d' % (805,650,50,0)) ; root.title("Маятник") ; root.iconbitmap("icon.ico") ; root.resizable(0, 0) #root.overrideredirect(1)
root.configure(bg='blue')
f1 = Frame(root, width=400, height=400, bg='SteelBlue1', bd=0); f1.place(x=0,y=0)#pack(side=LEFT)
C     = Canvas(root, width='400', height='400', bg='white') ; C.place(x=401,y=-4) #.pack(side=TOP)#.grid()

fig0 = plt.figure(figsize=[6.00, 2.0])
ax0 = fig0.add_subplot(1,1,1) #facecolor="red"
plt.subplots_adjust(right=0.75, left=0.7)

ax0.set_title('fi', fontsize=10 , y=1.0, pad=-10) #
ax0.set_xlabel("t, c", fontsize=10, labelpad=-5) # , labelpad=0  #ax1.xaxis.set_label_coords(10, 10)
#ax0.set_ylabel("Амп", fontsize=10, labelpad=-1) # , labelpad=0
ax0g = FigureCanvasTkAgg(fig0, master=root); #graph.place(x=0,y=401)#.pack(side=LEFT) #.grid()
ax0g.get_tk_widget().place(x=0, y=400, width=500, height=280)
x = np.array([0, 50])
y = np.array([-3, 3])
#self.F[0].xaxis.set_ticks(np.arange(0, self.div+self.sections*self.div+1, self.div))
line0, = ax0.plot(x, y, 'r-') # Returns a tuple of line objects, thus the comma
fig0.tight_layout()

fig1 = plt.figure(figsize=[4.05, 2.0])
ax1 = fig1.add_subplot(1,1,1) #facecolor="red"
ax1.set_title('Годограф', fontsize=10 , y=1.0, pad=-10) #
ax1.set_xlabel("Угол, рад", fontsize=10, labelpad=-5) # , labelpad=0  #ax1.xaxis.set_label_coords(10, 10)
ax1.set_ylabel("Угловая скорость", fontsize=10, labelpad=-1) # , labelpad=0
ax1g = FigureCanvasTkAgg(fig1, master=root); #graph.place(x=0,y=401)#.pack(side=LEFT) #.grid()
ax1g.get_tk_widget().place(x=490, y=400, width=320, height=280)
x = np.array([-2.0, 3.0])
y = np.array([-0.35, 0.35])
line1, = ax1.plot(x, y, 'r-') # Returns a tuple of line objects, thus the comma
fig1.tight_layout()

#ax1.clear()
#ax1g.draw()
#ax1.pie([xs,ys], autopct='%1.1f%%') # explode=explode, labels=labels, autopct='%1.1f%%', shadow=True, , startangle=90
#ax1.axis('equal')

Label(f1, text="Начальный угол", **menu_style, width=15, anchor='w').place(x=240, y=41)
e_ang = Entry(f1, width=5); e_ang.insert(10, 90);              e_ang.place(x=349, y=40)
e_ang.bind('<Return>', use_entered_ang); e_ang.bind('<FocusOut>', use_entered_ang)

Label(f1, text="dt", **menu_style, width=8, anchor='w').place(x=240,y=64)
dt_preset = {'Другой': -1, '0.001':0.001, '0.01':0.01, '0.1':0.1, '0.2':0.2, '0.3':0.3, '0.5':0.5, '1':1}
tkvar_dt = StringVar(root); tkvar_dt.set('0.2'); tkvar_dt.trace('w', change_dropdown_dt)
popupMenu_dt = OptionMenu(f1, tkvar_dt, *dt_preset); popupMenu_dt.config(width=6); popupMenu_dt.place(x=302,y=66)

e_dt = Entry( f1, width=9); e_dt.insert(10, 0.2); e_dt.place(x=240, y=79)
e_dt.bind('<Return>', use_entered_dt); e_dt.bind('<FocusOut>', use_entered_dt)

Label(f1, text="Выбор g", **menu_style, width=8,  anchor='w').place(x=240,y=105)
G_preset = {'Другой': -1, 'Земля':9.80665, 'Солнце':274.0, 'Юпитер':24.79, 'Нептун':11.15, 'Сатурн':10.445,
            'Уран':8.87,  'Марс':3.728, 'Луна':1.625, 'Плутон': 0.658, 'Рея':0.265 }
tkvar_g = StringVar(root); tkvar_g.set('Земля'); tkvar_g.trace('w', change_dropdown_g)
popupMenu_g = OptionMenu(f1, tkvar_g, *G_preset); popupMenu_g.config(width=6); popupMenu_g.place(x=302,y=107)
e_g = Entry(f1, width=9); e_g.insert(10, 9.80665); e_g.place(x=240, y=122);
e_g.bind('<Return>', use_entered_g); e_g.bind('<FocusOut>', use_entered_g)

Label(f1, text="Длина нити", **menu_style, width=10, anchor='w').place(x=240, y=144)
e_L = Entry(f1, width=10); e_L.insert(10, 250);              e_L.place(x=319, y=143);
e_L.bind('<Return>', use_entered_L); e_L.bind('<FocusOut>', use_entered_L)

Label(f1, text="λ", **menu_style, width=1, anchor='w').place(x=5,y=90)
sc = Scale(f1, from_=0, to=0.05, length=200, resolution=0.01, orient=HORIZONTAL) #tickinterval=0.01, command=tkvar_sc,
sc.bind("<ButtonRelease>", change_sc); sc.set(0.01); sc.place(x=20,y=80)

Label(f1, text="Алгоритм:", **menu_style, width=10,  anchor='w').place(x=5,y=130)
alg_preset = {'Эйлера':0, 'Верле': 1}
tkvar_alg = StringVar(root)
tkvar_alg.set('Верле')
popupMenu_alg = OptionMenu(f1, tkvar_alg, *alg_preset); popupMenu_alg.place(x=80,y=130)
popupMenu_alg.config(width=16)
tkvar_alg.trace('w', change_dropdown_alg)

#(     root, точка подвеса,            fi_0, нач.угл.скор.  R,    clr='blue')
O =Bob([C, [ax0, ax0g, line0], [ax1, ax1g, line1]], [sc, e_L, e_g, e_dt, e_ang], (int(C['width'])/2, 195), start_speed=0, R=20, clr='blue')

Label(f1, text="Форм. Гюйгенса T=", **menu_style, width=18, anchor='e').place(x=5,y=5)
Gugens_L=Label(f1, text=str(round(O.T_Gugens,10)), **menu_style, width=13, anchor='w'); Gugens_L.place(x=133,y=5)
b_pause = Button(f1, text=" Запустить ", width=19); b_pause.place(x=240, y=5); b_pause.bind("<Button-1>", pause);

Label(f1, text="Уточненное знач. T=", **menu_style, width=18, anchor='e').place(x=5,y=23)
Better_L=Label(f1, text=str(round(O.T_GugensPlus,10)), **menu_style, width=13, anchor='w'); Better_L.place(x=133,y=23)
Label(f1, text="Вычисленное K(k) T=", **menu_style, width=18, anchor='e').place(x=5,y=41)
Calcul_L=Label(f1, text=str(round(O.T_K,10)), **menu_style, width=13, anchor='w'); Calcul_L.place(x=133,y=41)
#засечки по точке равновейсия
Label(f1, text="По  равновейсию  T=", **menu_style, width=18, anchor='e').place(x=5,y=59)
Zasech_L=Label(f1, text="", **menu_style, width=13, anchor='w'); Zasech_L.place(x=133,y=59)  # засечки по точке равновейсия

root.after(animate_speed, m)
root.mainloop()

#a[y][x].bind("<Button-1>", lambda event, x=x, y=y: but_one_press(event, x, y) )
#a[y][x].grid(row=y, column=x, padx=1, pady=1, sticky = 'nesw')
#a[y][x].cord = (x,y)
#but_one_press(event, x, y)


#
#[print(i) for i in dir(ax1.set_xlabel)]
#b_g = Button(f1, text="Ок"); b_g.place(x=260, y=270); b_g.bind("<Button-1>", use_entered_g);
#b_dt = Button(f1, text="Ок"); b_dt.place(x=260, y=305); b_dt.bind("<Button-1>", use_entered_dt);
#b_L=Button(f1, text="Ок");       b_L.place(x=260, y=340); b_L.bind("<Button-1>", use_entered_L);
#b1=Button(f1, text="Годограф");  b1.place(x=5,y=380); b1.bind("<Button-1>", Rest)
#b2=Button(f1, text="График fi"); b2.place(x=80,y=380); b2.bind("<Button-1>", Rest2)
