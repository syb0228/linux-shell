# shell-proj

### Unix shell project
- 2020년 11월 프로젝트
- shell 만들기

Requirement
- chdir() 처리
- exit() 처리
- SIGCHID로 자식 프로세스 wait()시 프로세스가 수행되도록 구현
- ^C(SIGINT), ^/(SIGQUIT) 사용시 쉘이 종료되지 않도록 구현
- 포그라운드 프로세스 실행시 SIGINT를 받으면 프로세스가 끝나도록 구현
- 백그라운드 실행 (명령 &)
- Signal 처리
- Redirection 처리
- Pipe, | 처리

### Linux shell project
- 2022년 10월 프로젝트
- bash에서 input text 파일을 처리하기 위한 script program
Make your script program for handling a 
input text file in BASH

Requirement
- Enable/disable empty line removal
- Enable/disable comment removal
- Enable/disable duplicate whitespaces among words
- Add the line number 
- Change the variable name
- Remove ${} in arithmetic expansion
- Export new file
- Exit
