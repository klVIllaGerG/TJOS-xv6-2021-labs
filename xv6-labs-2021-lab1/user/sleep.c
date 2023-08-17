#include "kernel/types.h"
#include "user/user.h"

const int duration_pos = 1;
typedef enum {wrong_char, success_parse, toomany_char} cmd_parse;
cmd_parse parse_cmd(int argc, char** argv);

int 
main(int argc, char** argv){
    //printf("%d, %s, %s \n",argc, argv[0], argv[1]);
    if(argc == 1){
        printf("Please enter the parameters!");  // 提示用户输入参数
        exit(1);
    }
    else{
        cmd_parse parse_result;
        parse_result = parse_cmd(argc, argv);  // 解析参数
        if(parse_result == toomany_char){
            printf("Too many args! \n");  // 参数过多
            exit(1);
        }
        else if(parse_result == wrong_char){
            printf("Cannot input alphabet, number only \n");  // 输入了非数字字符
            exit(1);
        }
        else{
            int duration = atoi(argv[duration_pos]);
            //printf("Sleeping %f", duration / 10.0);
            sleep(duration);  // 进行休眠
            exit(1);
        }
        
    }
    exit(0);
}

cmd_parse
parse_cmd(int argc, char** argv){
    if(argc > 2){
        return toomany_char;  // 参数过多
    }
    else {
        int i = 0;
        while (argv[duration_pos][i] != '\0')
        {
            /* code */
            if(!('0' <= argv[duration_pos][i] && argv[duration_pos][i] <= '9')){
                return wrong_char;  // 输入了非数字字符
            }
            i++;
        }
        
    }
    return success_parse;  // 参数解析成功
}