#!/bin/bash

echo "[Source Code]"

cat $1

echo "-----------------------------------
User Name : ShinYebin 12181623
[MENU]
1.Enable/disable empty line removal
2. Enable/disable comment removal
3. Enable/disable duplicate whitespaces among words
4. Add the line number
5. Change the variable name
6. Remove \${} in arithmathic expansion
7. Export new file
8. Exit
---------------------------------------"
echo "Enter your choice [1-8]" 
read choice

choice_list=()

oldname_list=()
newname_list=()

cnt1=0
cnt2=0

while [ $choice -lt 8 ]
do
	if [ $choice = 1 ]
       	then
		echo "Do you want to remove all blank line?(y/n) :"
		read check
		if [ $check = "y" ]
		then
			sed '/^$/d' $1
			choice_list+=(1)
		fi
	elif [ $choice = 2 ]
	then
		echo "Do you want to remove all comment?(y/n) :"
		read check
		if [ $check = "y" ]
		then
			sed '/^\# /d' $1
			choice_list+=(2)
		fi
	elif [ $choice = 3 ]
	then
		echo "Do you want to remove duplicate whitespaces?(y/n) :"
		read check
		if [ $check = "y" ]
		then
			sed 's/[ ]\+/ /g' $1
			choice_list+=(3)
		fi
	elif [ $choice = 4 ]
	then
		echo "Where you want to add whether the start or the end of the line. (s/e) : "
		read check
		if [ $check = "s" ]
		then
			awk '{ print "#" NR " " $s}' $1
			choice_list+=(4)
		elif [ $check = "e" ]
		then
			awk '{ print $s " #" NR}' $1
			choice_list+=(5)
		fi
	elif [ $choice = 5 ]
	then
		echo "Enter the variable to be changed and the new variable name : "
		echo "Variable name to be changed : "
		read oldname
		echo "New variable name : "
		read newname
		sed -E "s/$oldname/$newname/g" $1
		choice_list+=(6)
		oldname_list+=($oldname)
		newname_list+=($newname)
	elif [ $choice = 6 ]
	then
		echo "Do you want to remove \${}(y/n) : "
		read check
		if [ $check = "y" ]
		then
			sed -E 's/(\$\{)(.*)(})/\2/' $1
			choice_list+=(7)
		fi
	elif [ $choice = 7 ]
	then
		for (( i=$cnt1; i<${#choice_list[@]}; i++ )); do
			if [ ${choice_list[i]} = 1 ]
			then
				sed -i '/^$/d' $1
			elif [ ${choice_list[i]} = 2 ]
			then
				sed -i '/^\# /d' $1
			elif [ ${choice_list[i]} = 3 ] 
			then
				sed -i 's/[ ]\+/ /g' $1
			elif [ ${choice_list[i]} = 4 ]
			then
				awk '{ print "#" NR " " $s}' $1 > tmp && mv tmp $1
			elif [ ${choice_list[i]} = 5 ]
			then
				awk '{ print $s " #" NR}' $1 > tmp && mv tmp $1
			elif [ ${choice_list[i]} = 6 ]
			then 
				sed -i -E "s/${oldname_list[$cnt2]}/${newname_list[$cnt2]}/g" $1
				cnt2+=1
			elif [ ${choice_list[i]} = 7 ]
			then
				sed -i -E 's/(\$\{)(.*)(})/\2/' $1
			fi
			cnt1+=1
		done
	fi
	
	echo "Enter your choice [1-8]"
        read choice
done
