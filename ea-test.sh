#!/bin/bash

EA_TEST_FILE=ea-test
SAMPLE_COUNT=10
NAME_PREFIX="test"
SAMPLE_PATH="/tmp/ea-test-sample"
SAMPLE_MAXSIZE=4096
SAMPLE_SIZE_ARRAY=()
SAMPLE_RET=()
SAMPLE_SHA1SUM=()

usage()
{
	printf "Usage: %s <Mountpoint>" $(basename $0)
	exit 1
}

if [[ $# != 1 ]];then
	usage
fi

MP=$1

if [[ ! -d $MP ]];then
	printf "Non-existing mountpoint %s !" $MP
	exit 2
fi

mkdir -p $SAMPLE_PATH
touch $MP/$EA_TEST_FILE
for ((i=0;i<SAMPLE_COUNT;i++));do
	SAMPLE_SIZE_ARRAY[i]=$(expr ${RANDOM} % $SAMPLE_MAXSIZE)
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w ${SAMPLE_SIZE_ARRAY[i]} | head -n 1 > $SAMPLE_PATH/${NAME_PREFIX}-$SAMPLE_COUNT
	SAMPLE_SHA1SUM[i]=$(sha1sum $SAMPLE_PATH/${NAME_PREFIX}-$SAMPLE_COUNT | awk '{print $1}')
	RETSTR=$(setfattr -n "user.${NAME_PREFIX}-${i}" -v `cat $SAMPLE_PATH/${NAME_PREFIX}-$SAMPLE_COUNT` $MP/$EA_TEST_FILE 2>&1)
	SAMPLE_RET[i]=$?
	if [[ RETSTR == "" ]];then
		RETSTR="OK"
	fi
	printf "%s: %s\n" user.${NAME_PREFIX}-${i} "${RETSTR}"
done

for ((i=0;i<SAMPLE_COUNT;i++));do
	if [[ SAMPLE_RET[i] == 0 ]];then
		DATA=`getfattr -n user.${NAME_PREFIX}-${i} $MP/$EA_TEST_FILE`
		SHA1SUM=$(echo DATA | sha1sum - | awk '{print $1}')
		if [[ SHA1SUM != SAMPLE_SHA1SUM[i] ]];then
			printf "%s: Expected sha1sum %s, result %s\n" user.${NAME_PREFIX}-${i} ${SAMPLE_SHA1SUM[i]} $SHA1SUM
		else
			printf "%s: Result correct\n" user.${NAME_PREFIX}-${i}
		fi
		setfattr -x "$user.${NAME_PREFIX}-${i}"
	else
		printf "%s: Result correct\n" user.${NAME_PREFIX}-${i}
	fi
done

rm -rf $SAMPLE_PATH

exit 0
