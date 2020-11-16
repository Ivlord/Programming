#
# Pathfind v.2.0 (c) Мазлов Иван
#  - убран class Path
#  - ускорен алгоритм проверки новых точек с помощью set-ов
#  - добавлена возможность разных видов препятствий
#  - почищен код
#
class pf(): #       tuples=> (x,y) (x,y)      Оne step:    Up    Right   Down   Left      Стенки и препятствия
    def __init__(self, maze, start, end, update_model = ((0,-1), (1,0), (0,1), (-1,0)), blocks = [1, '#', '@']):
        self.found, self.end, self.maze, self.update_model, self.paths = [], end, maze, update_model, [[start]]
        self.upd_len        = len(self.update_model)                   #      path0                path1
        self.dot_sum        = lambda A, B: (A[0] + B[0], A[1] + B[1]) # [ [(x0,y0),(x0,y0)], [(x,y),... ,(x,y)]]
        self.good = set([(x, y) for y in range(len(maze)) for x in range(len(maze[y])) if maze[y][x] not in blocks])

    def do(self): # Полностью прокручивает весь поиск пути
        while self.paths and not self.found: self.paths = self.one_step([])
        return self.found

    def one_step(self, new_paths = []): # Плюс один шаг на всех "живых" путях (добавляет к каждому пути ещё точки)
        for path in self.paths:
            new_dots = set(map(self.dot_sum, [path[-1]] * self.upd_len, self.update_model)) & self.good
            self.good -= new_dots
            if self.end in new_dots:
                self.found = path + [self.end]
                break
            new_paths += [path + [dot] for dot in new_dots]
        return new_paths

def Z_22():
    maze = [#012345678901234567890123456
            "####B######################", #00
            "# #       #   #      #    #", #01
            "# # # ###### #### #######@#", #02 Добавлена стена @ : 23,1 - Выхода нет
            "# # # #       #   #       #", #03
            "#   # # ######### # ##### #", #04
            "# # # #   #       #     # #", #05
            "### ### ### ############# #", #06
            "# #   #     # #           #", #07
            "# # #   ####### ###########", #08
            "# # # #       # #         C", #09
            "# # ##### ### # # ####### #", #10
            "# # #     ### # #       # #", #11
            "#   ##### ### # ######### #", #12
            "######### ### #           #", #13 Только выход C: 22,13
            "# ####### ### #############", #14
            "A           #   ###   #   #", #15
            "# ############# ### # # # #", #16
            "# ###       # # ### # # # #", #17
            "# ######### # # ### # # # F", #18
            "#       ### # #     # # # #", #19
            "# ######### # ##### # # # #", #20
            "# #######   #       #   # #", #21
            "# ####### ######### #######", #22
            "#         #########       #", #23
            "#######E############D######"    #   len()=27x25
        ]

    while True:
        x, y = SecureInput([[int], [int]], hlp = 'Введите Ваше местоположение (x,y): ')
        if (not 0 <= y < len(maze)) or (not 0 <= x < len(maze[0])): # TODO: maze[0] =>maze[start[1]]
            print('Неверные координаты (за пределами лабиринта).\n\n')
        elif maze[y][x] in ['#', 1]:
            print('Не верные координаты (в стене).\n\n')
        else: break
    have_exits=[maze[ex[1]][ex[0]] for ex in maze_exits(maze) if pf(maze, (x, y), ex).do()]
    print('Доступны выходы: ' + ' '.join(sorted(have_exits)) if have_exits else 'Выхода нет')

def maze_exits(maze, exit_marks=['ABCDEFGHIJKLMNOPQRSTUVWXYZ', 20]):
    return [(x,y) for y in range(len(maze)) for x in range(len(maze[y]))
                  if maze[y][x] in exit_marks[0] or maze[y][x] in exit_marks]
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
