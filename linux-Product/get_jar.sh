SOURCE=`pwd`
OFFICE_JAR=TIOffice.jar
rm ${OFFICE_JAR}

cd ../workspace/tsoffice
rm ${OFFICE_JAR}
./to_jar.sh

cp ${OFFICE_JAR} ${SOURCE}

