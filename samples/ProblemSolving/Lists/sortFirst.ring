aList = [[2,2], [1,2], [3,2], [3,1], [1,1]]
sortFirst(aList)

for n=1 to len(aList)
      for m=1 to 2
            see string(aList[n][m]) + " "
      next
      see nl
next

func sortFirst aList
        aList = sort(aList,1)

        for n=1 to len(alist)-1
             for m=n to len(aList)-1 
                   if alist[m+1][1] = alist[m][1] and alist[m+1][2] < alist[m][2]
                      temp = alist[m+1]
                      alist[m+1] = alist[m]
                      alist[m] = temp ok
             next
       next
       return aList
