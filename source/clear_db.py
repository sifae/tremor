import sqlite3
import os

def get_parent_dir(directory):
    return os.path.dirname(directory)

parent = get_parent_dir(os.getcwd())
os.chdir("{0}/data".format(parent))

conn = sqlite3.connect("data.db")
cursor = conn.cursor()

sql_1 = "DELETE FROM esp"
sql_2 = "DELETE FROM opencv"

cursor.execute(sql_1)
cursor.execute(sql_2)
conn.commit()