import string
import fileinput, sys
versionNumber =""

headerfilepath = "AutoIncrement.h"
buildtag = "VERSION_BUILD_FILE"

for line in fileinput.input(headerfilepath,inplace=True):
    row = line.rstrip()
    if buildtag in row:
        number = [int(s) for s in row.split(' ') if s.isdigit()]
        if len(number) > 0:
            buildnumber = number[0]
            buildnumber += 1;
            versionNumber = row.replace(str(number[0]),str(buildnumber))
            print(versionNumber)
            line.replace(row,versionNumber)
        else:
            print(row)
    else:
        print(row)

