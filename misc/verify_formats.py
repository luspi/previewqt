##########################################################################
##                                                                      ##
## Copyright (C) 2024-2026 Lukas Spies                                  ##
## Contact: https://previewqt.org                                       ##
##                                                                      ##
## This file is part of PreviewQt.                                      ##
##                                                                      ##
## PreviewQt is free software: you can redistribute it and/or modify    ##
## it under the terms of the GNU General Public License as published by ##
## the Free Software Foundation, either version 2 of the License, or    ##
## (at your option) any later version.                                  ##
##                                                                      ##
## PreviewQt is distributed in the hope that it will be useful,         ##
## but WITHOUT ANY WARRANTY; without even the implied warranty of       ##
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        ##
## GNU General Public License for more details.                         ##
##                                                                      ##
## You should have received a copy of the GNU General Public License    ##
## along with PreviewQt. If not, see <http://www.gnu.org/licenses/>.    ##
##                                                                      ##
##########################################################################

import sqlite3
import os

# build up database of formats in source file

srcdata = {}

srcpaths = ["../cplusplus/files/plugins", "../cplusplus/files/plugins/images"]

# flatten a list
def flat(lis):
    flatList = []
    # Iterate with outer list
    for element in lis:
        if type(element) is list:
            # Check if type is list than iterate through the sublist
            for item in element:
                flatList.append(item)
        else:
            flatList.append(element)
    return flatList

# compose dictionary of all formats in the source files
for srcpath in srcpaths:
    for ele in os.scandir(srcpath):
        if ele.is_file():
            with open(ele.path, "r") as f:

                txt = f.read()

                if "// ~~ FILETYPES ~~" not in txt:
                    continue

                frmts = txt.split("// ~~ FILETYPES ~~")[1].split("// ~~~~")[0].split("\n")
                frmts = list(filter(None, frmts))

                for fr in frmts:

                    line = fr.strip()

                    if not line.startswith("{"):
                        continue

                    uniqueid = line.split(",")[0].split("{")[1]
                    desc = line.split("{{\"")[1].split("\"},")[0]
                    endgs = line.split("{\"")[2].split("\"}")[0].split("\", \"")
                    endgs = [e.split("\",\"") for e in endgs]
                    endgs = flat(endgs)

                    if uniqueid in srcdata and (srcdata[uniqueid][0] != desc or srcdata[uniqueid][1] != endgs):
                        print(f"!! Missmatch for id {uniqueid} in file {ele.name} // {srcdata[uniqueid][1]} // {endgs}")
                    else:
                        srcdata[uniqueid] = [desc,endgs]

conn = sqlite3.connect('fileformats.db')
c = conn.cursor()

some_problem = False

# check for any format not in the database (or incomplete/different)
for uniqueid in srcdata:

    # get all data
    c.execute('SELECT endings,description FROM imageformats WHERE uniqueid=?', (uniqueid,))
    data = c.fetchall()
    if len(data) == 0:
        print(f"ERROR: format not in database: {uniqueid} / {srcdata[uniqueid]}")
        some_problem = True
    elif data[0][1] != srcdata[uniqueid][0] or data[0][0].split(",") != srcdata[uniqueid][1]:
        print(f"ERROR: missmatched format: {uniqueid} / {srcdata[uniqueid]}")
        some_problem = True


# check for any format in the database not part of any plugin
c.execute("SELECT uniqueid FROM imageformats")
data = c.fetchall()
for entry in data:
    if str(entry[0]) not in srcdata:
        print(f"db format not in any source file, uniqueid: {entry[0]}")
        some_problem = True

if not some_problem:
    print("no problems found, formats appear coherent")
