
# ******************************************************************************************
#
class Path():
    # Полный путь между двумя точками
    # dest - конечная точка (x,y), parent_path - путь от начальной точки до развеетвления
    def __init__(self, dest, parent_path, parent_pf):
        self.dots = parent_path if isinstance(parent_path, list) else [parent_path]
        self.dest = dest
        self.p    = parent_pf

    # Проверка точки на то, что она уже есть в этом пути. False='да есть'
    def check_dot(self, dot):
        return not (dot in self.dots)

    # Создаем 4 разветвления из узла, убираем "мертвые" направления
    def update(self):
        path_list=[ Path(self.dest, self.dots+[dot], self.p)
            for dot in self.p.create_new_dots(self.dots[-1]) if
            self.p.check_in_range(dot)       and           # новая точка не вылезна за рамки лабиринта
            self.p.check_maze_obstacle(dot)  and           # не занята ли
            self.p.check_dot_is_unique(dot)                # точка уже проверялась в другом пути
            ] or [Path(self.dest, self.dots+[False], self.p)]

        # Проверка, что мы попали в требуемую конечную точку
        find_dest=[path for path in path_list if path.dots[-1]==self.dest]
        for i in find_dest: i.dots = i.dots + [True]
        closed_paths=[path for path in path_list if path.dots[-1]==False]
        self.p.remove_pathes(path_list,  find_dest)
        self.p.remove_pathes(path_list, closed_paths)

        return path_list, find_dest, closed_paths, [self]

class pf():
    def __init__(self, maze, start, end, update_model=((0,-1),(1,0),(0,1),(-1,0)) ):
        self.step = 0
        self.start, self.end = start, end
        self.maze            = maze
        self.maze_max_y, self.maze_max_x = len(self.maze) - 1, len(self.maze[0]) - 1
        self.update_model    = update_model
        self.paths           = [Path(end, start, self)]
        self.blocked_paths, self.found_paths                    = [], []
        self.t_paths, self.t_blocked, self.t_found, self.t_kill = [], [], [], []

    def do(self):
        number_of_active_paths = True
        while number_of_active_paths:
            number_of_active_paths, found = self.one_step()
            if found: break
        if found: return found[0].dots
        else:     return False

    def one_step(self):  #return open_paths, find_dest, closed_paths, self
        self.step =self.step + 1
        self.t_paths, self.t_blocked, self.t_found, self.t_kill = [], [], [], []
        for path in self.paths:
            o, f, b, k = path.update()
            self.t_paths       = self.t_paths       + o
            self.t_blocked     = self.t_blocked     + b
            self.t_found       = self.t_found       + f
            self.t_kill        = self.t_kill        + k
        self.remove_pathes(self.paths, self.t_blocked + self.t_found + self.t_kill)
        self.paths = self.paths + self.t_paths
        self.found_paths   = self.found_paths   + self.t_found
        self.blocked_paths = self.blocked_paths + self.t_blocked
        return len(self.paths), self.t_found

    # Проверяем, что точка ещё нигде не засветилась в других путях и не обработана раньше
    def check_dot_is_unique(self, dot):
        return all(path.check_dot(dot) for lst in [
            self.paths,   self.blocked_paths, self.found_paths,
            self.t_paths, self.t_blocked, self.t_found, self.t_kill] for path in lst)

    # Разветвления от текущей точки
    def create_new_dots(self, parent_dot):
        return [(parent_dot[0] + dot[0], parent_dot[1] + dot[1]) for dot in self.update_model]

    # убираем список "заглохших" маршрутов из общего списка маршрутов
    def remove_pathes(self, main_list, del_list):
        for path in del_list:
            if path in main_list: del main_list[main_list.index(path)]

    # Проверяем свободно ли место там, где мы добавили новую точку
    def check_maze_obstacle(self, dot):
        return self.maze[dot[1]][dot[0]]==0

    # Проверяем не вылазит ли точка за границы лабиринта
    def check_in_range(self, dot):
        return 0 <= dot[0] <= self.maze_max_x and 0 <= dot[1] <= self.maze_max_y

def dmp(dmp, t1, t2):
    global dbg
    if not dbg: return
    print(t1, '  =', len(dmp), ':')
    for path in dmp: print('   ', len(path.dots), path.dots)
    if t2 != '': print(t2)

def fill_maze(maze, path):
    # fill the maze
    if path:
        for dot in path:
            if isinstance(dot, (list, tuple)):
                maze[dot[1]][dot[0]]=8
    #print out the maze
    for a in maze:
        print(a)

def maze_str2int(maze_ald):
    exits={}
    maze=[]
    for y in range(len(maze_ald)):
        new_row=[]
        for x in range(len(maze_ald[y])):
            if maze_ald[y][x] in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ':
                new_row.append(0)
                exits[maze_ald[y][x]]=[x,y] # запоминаем выход
            elif maze_ald[y][x]==' ':
                new_row.append(0)
            elif maze_ald[y][x]=='#':
                new_row.append(1)
        maze.append(new_row)
    return maze, exits

def maze_clear_steps(maze):
    return [ [0 if maze[y][x]==8 else maze[y][x] for x in range(len(maze[y]))] for y in range(len(maze))]


def Z_22():
    print('============')
    print('= Лабиринт =')
    print('============\n')

    maze = [
        "####B######################",
        "# #       #   #      #    #",
        "# # # ###### #### ####### #",
        "# # # #       #   #       #",
        "#   # # ######### # ##### #",
        "# # # #   #       #     # #",
        "### ### ### ############# #",
        "# #   #     # #           #",
        "# # #   ####### ###########",
        "# # # #       # #         C",
        "# # ##### ### # # ####### #",
        "# # #     ### # #       # #",
        "#   ##### ### # ######### #",
        "######### ### #           #",
        "# ####### ### #############",
        "A           #   ###   #   #",
        "# ############# ### # # # #",
        "# ###       # # ### # # # #",
        "# ######### # # ### # # # F",
        "#       ### # #     # # # #",
        "# ######### # ##### # # # #",
        "# #######   #       #   # #",
        "# ####### ######### #######",
        "#         #########       #",
        "#######E############D######"    #   27x25
    ]

    maze, exits=maze_str2int(maze)
    #exit_location={'B':[4, 0], 'C':[26, 9], 'A':[0, 15], 'F':[26, 18], 'E':[7, 24], 'D':[20, 24]}
    #fill_maze(maze, [])

    while True:
        start_pos=tuple(SecureInput([[int],[int]], hlp='Введите Ваше местоположение (x,y): '))
        if (not 0<=start_pos[1]<len(maze)) or (not 0<=start_pos[0]<len(maze[0])):
            print('Не верные координаты (за пределами лабиринта).\n\n')
            continue
        elif maze[start_pos[1]][start_pos[0]]==1:
            print('Не верные координаты (в стене).\n\n')
            continue

        dbg=True
        good_exits=[]
        for e in exits:
            pf1=pf(maze, start_pos, tuple(exits[e]))  #(1,14)
            result=pf1.do()
            if result:
                good_exits.append(e)
        if good_exits:
            print('Доступны выходы:', ' '.join(sorted(good_exits))   )
        else:
            print('Выхода нет')
        break

    #print()
    #fill_maze(maze, result)
    #maze=maze_clear_steps(maze)
    #print()
    #fill_maze(maze, [])


#
##################################################################################
#
def FloatToIntNoLoseCheck(x):
    return int(x) if not (x-int(x)) else x

# function   range  function   NO range, если любые числа
# a:[ [int, [1, 2],  [float] ]
def SecureInput(a, separ=' ', hlp='', clr='(),'):
    if not isinstance(a, list) or not a: return []
    while True:
        raw = input(hlp)
        for i in clr: raw = raw.replace(i, ' ')
        raw = raw.split() if separ==' ' else raw.split(separ)
        try:
            if len(raw)>len(a): raise Exception('   введены лишние данные\n')
            res=[a[i][0](raw[i]) for i in range(len(a))
                if len(a[i])==1 or a[i][1][0] <= a[i][0](raw[i]) <= a[i][1][1]]
            if len(res)<len(a): raise Exception('   ошибка диапазона\n') # что-то из данных вне диапазона
            break

        except LookupError:     print('   не все данные введены\n')
        except ValueError:      print('   ошибка типа данных.\n')
        except Exception as e:  print(e)
    return res


if __name__ == "__main__":
    Z_22()
