import json

def full_write():

    with open("in.json", "r") as read_file:
        data = json.load(read_file)
    out=[]
    for i in data:
        if not out or out[-1]['userId']!=i['userId']:
            out.append({'task_completed':0, 'userId':i['userId']})
        if i['completed']: out[-1]['task_completed']+=1

    with open("out.json", "w") as write_file:
        json.dump(out, write_file)

# Данная версия будет обрабатывать json файл даже, если иды будут идти несортированно (как у Вас в файле in.json)
def full_write2():

    with open("in.json", "r") as read_file:
        data = json.load(read_file)
    db, out = {}, []
    for i in data:
        if i['userId'] not in db:
            db[i['userId']]=0
        elif i['completed']: db[i['userId']]+=1
    for i in sorted(list(db)):
        out.append({'task_completed':db[i], 'userId':i})
    with open("out.json", "w") as write_file:
        json.dump(out, write_file)
