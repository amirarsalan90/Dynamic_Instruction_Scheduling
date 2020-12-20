#include <iostream>
#include <sstream>
#include <queue>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

struct file_line{
    int tag;
    string pcounter;
    int optype;
    int destreg;
    int src1;
    int src2;
    int counter;
    char state;
};

struct rob_struct{
    int optype;
    int destreg;
    int src1;
    int src2;
    int fetch_t;
    int disp_exit;
    int issue_exit;
    int exec_exit;
};

struct file_line1{
    int tag;
    string pcounter;
    int optype;
    int destreg;
    int src1;
    int src2;
    int counter;
    char state;
    int dep_inst1;
    int dep_inst2;
};

struct rob_struct1{
    int optype;
    int destreg;
    int src1;
    int src2;
    int fetch_t;
    int disp_exit;
    int issue_exit;
    int exec_exit;
};

bool operator==(file_line1 const & a,file_line1 const & b)
{
    return a.pcounter == b.pcounter and a.optype == b.optype;
}

bool operator==(file_line const & a,file_line const & b)
{
    return a.pcounter == b.pcounter and a.optype == b.optype;
}


int main(int argc, char *argv[]) {

    char *p2arg, *p3arg;
    int S = strtol(argv[1], &p2arg, 10);
    int N = strtol(argv[2], &p3arg, 10);
    string filename = argv[3];

    if (S == 64 && N ==1){


        map<int,int> RF;
        deque<file_line1> all_lines;
        map<int,rob_struct1> fakerob;

        //initializing RF
        for (int i=-1; i<128; i++){
            RF[i] = -1;
        }


        ifstream file (filename);
        string line;

        int mytag = 0;
        int theline;
        while (getline(file, line)){

            //cout << line<<endl;
            vector<string> line_separated;
            string line_element;
            istringstream s(line);
            while (getline(s, line_element, ' ')){
                line_separated.push_back(line_element);
            }
            all_lines.push_back(file_line1{mytag,line_separated.at(0), stoi(line_separated.at(1)), stoi(line_separated.at(2)), stoi(line_separated.at(3)), stoi(line_separated.at(4)), 0, 'N',-2,-2 });
            //if (mytag == 1839){
            //    cout << line << "\n\n";
            //}

            //if (mytag == 1841){
            //    cout << line << "\n\n";
            //}
            fakerob.insert(pair<int,rob_struct1>(mytag, rob_struct1{stoi(line_separated.at(1)), stoi(line_separated.at(2)), stoi(line_separated.at(3)),stoi(line_separated.at(4)),0,0,0,0}));
            mytag += 1;
        }




        vector<file_line1> fetch_que;
        vector<file_line1> dispatch_que;
        vector<file_line1> schedule_que;
        vector<file_line1> execute_que;

        int available_FU;
        int available_schedule;
        int available_dispatch;
        int dispatch_bandwidth;


        int cycle = 0;
        while (!all_lines.empty() || !execute_que.empty() || !schedule_que.empty() || !dispatch_que.empty()){

            //increment counters
            if (!execute_que.empty()){
                for (auto it = execute_que.begin(); it != execute_que.end(); it++){
                    it->counter += 1;//it->counter + 1;
                }
            }






            /////moving from execution to WB results

            if (!execute_que.empty()){

                for (auto it = execute_que.begin(); it != execute_que.end(); it++){

                    if (it -> optype == 0){
                        if (it -> counter == 1){
                            it -> state = 'W';
                        }
                    }
                    if (it -> optype == 1){
                        if (it -> counter == 2){
                            it -> state = 'W';
                        }
                    }
                    if (it -> optype == 2){
                        if (it -> counter == 5){
                            //cout << "kir" << endl;
                            it -> state = 'W';
                        }
                    }
                }
            }


            ///////////processing execute_list
            //remove 'W's
            if (!execute_que.empty()){
                vector<file_line1> v_temp;
                for (auto it = execute_que.begin(); it != execute_que.end(); it++){
                    if (it -> state == 'W' ) {

                        auto itm = fakerob.find(it->tag);
                        itm->second.exec_exit = cycle;

                        if (RF[it->destreg] == it->tag){
                            RF[it->destreg] = -1;
                        }

                        v_temp.push_back(file_line1{it->tag,it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'W',it->dep_inst1, it->dep_inst2});
                    }
                }
                for (auto it=v_temp.begin(); it!=v_temp.end();it++){
                    //cout << "writebacked: " << it->tag << endl;
                    execute_que.erase(remove(execute_que.begin(),execute_que.end(), file_line1{it->tag,it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,it->state, it->dep_inst1, it->dep_inst2}), execute_que.end());
                }
            }





            available_FU = N + 1;
            /////from schedule to execute list (issuing)
            if (!schedule_que.empty()){
                vector<file_line1> v_temp;
                for (auto it = schedule_que.begin(); it != schedule_que.end(); it++){
                    if (available_FU > 0) {
                        if ((it -> src1 != -1) && (it -> src2 != -1)){

                            if (it->dep_inst1 == -1 && it->dep_inst2 == -1){
                                execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                auto itm = fakerob.find(it->tag);
                                itm->second.issue_exit = cycle;
                                v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                available_FU -= 1;

                            } else if(it->dep_inst1 == -1 && it->dep_inst2 != -1){

                                auto operand2 = fakerob.find(it->dep_inst2);
                                if (operand2->second.exec_exit > 0){
                                    execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                    auto itm = fakerob.find(it->tag);
                                    itm->second.issue_exit = cycle;
                                    v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                    available_FU -= 1;
                                }

                            } else if(it->dep_inst1 != -1 && it->dep_inst2 == -1){

                                auto operand1 = fakerob.find(it->dep_inst1);
                                if (operand1->second.exec_exit > 0){
                                    execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                    auto itm = fakerob.find(it->tag);
                                    itm->second.issue_exit = cycle;
                                    v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                    available_FU -= 1;
                                }

                            } else if(it->dep_inst1 != -1 && it->dep_inst2 != -1){

                                auto operand1 = fakerob.find(it->dep_inst1);
                                auto operand2 = fakerob.find(it->dep_inst2);
                                if (operand1->second.exec_exit > 0 && operand2->second.exec_exit > 0){
                                    execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                    auto itm = fakerob.find(it->tag);
                                    itm->second.issue_exit = cycle;
                                    v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                    available_FU -= 1;
                                }

                            }



                        } else if (it -> src1 != -1 && it -> src2 == -1){

                            if (it->dep_inst1 == -1){
                                execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                auto itm = fakerob.find(it->tag);
                                itm->second.issue_exit = cycle;
                                v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                available_FU -= 1;
                            } else if(it->dep_inst1 != -1){
                                auto operand1 = fakerob.find(it->dep_inst1);
                                if (operand1->second.exec_exit > 0){
                                    execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                    auto itm = fakerob.find(it->tag);
                                    itm->second.issue_exit = cycle;
                                    v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                    available_FU -= 1;
                                }
                            }


                        } else if ( it -> src1 == -1 && it -> src2 != -1){

                            if (it->dep_inst2 == -1){
                                execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                auto itm = fakerob.find(it->tag);
                                itm->second.issue_exit = cycle;
                                v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                available_FU -= 1;
                            } else if(it->dep_inst2 != -1){
                                auto operand2 = fakerob.find(it->dep_inst2);
                                if (operand2->second.exec_exit > 0){
                                    execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                                    auto itm = fakerob.find(it->tag);
                                    itm->second.issue_exit = cycle;
                                    v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                                    available_FU -= 1;
                                }
                            }


                        } else if (it -> src1 == -1 && it -> src2 == -1){
                            execute_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E', it->dep_inst1, it->dep_inst2});
                            //////////////////////////////////////////////update fakerob
                            auto itm = fakerob.find(it->tag);
                            itm->second.issue_exit = cycle;

                            /*if (it -> destreg != -1){
                                RF[it -> destreg] = 'n';
                            }*/
                            v_temp.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S', it->dep_inst1, it->dep_inst2});
                            available_FU -= 1;

                        }
                    }
                }
                for (auto it=v_temp.begin(); it!=v_temp.end();it++){
                    //cout << "executed: " <<it->tag << endl;
                    schedule_que.erase(remove(schedule_que.begin(),schedule_que.end(), file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,it->state, it->dep_inst1, it->dep_inst2}), schedule_que.end());
                }

            }




            //////from Dispatch to Schedule list
            /*
            auto it = dispatch_que.begin();
            for (it = dispatch_que.begin(); it != dispatch_que.end(); it++){
                if (it->state == 'D'){
                    break;
                }
            }
            */
            available_schedule = S - schedule_que.size();
            dispatch_bandwidth = N;
            while (dispatch_bandwidth > 0){
                if (available_schedule > 0){
                    if (!dispatch_que.empty()){
                        auto itt = dispatch_que.begin();
                        if (itt->state == 'D'){
                            //cout << "kiir" << endl;
                            auto it = dispatch_que.begin();

                            schedule_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S',RF[it->src1], RF[it->src2]});

                            //cout << "dispatched: " << it->tag << endl;

                            //////////////////////////////////////////////update fakerob
                            auto itm = fakerob.find(it->tag);
                            itm->second.disp_exit = cycle;

                            if (it->destreg != -1){
                                RF[it->destreg] = it->tag;
                            }

                            dispatch_que.erase(it);
                            it = dispatch_que.begin();
                            available_schedule -= 1;
                            dispatch_bandwidth -= 1;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }



            ////Fetch state to Dispatch state
            if (!dispatch_que.empty()){

                for (auto it = dispatch_que.begin(); it != dispatch_que.end(); it++){
                    if (it->state == 'F'){
                        it->state = 'D';

                    }
                }
            }



            ////processing fetch list
            for (int i=0 ; i < N; i++){

                if (!all_lines.empty() && dispatch_que.size() < (2 * N)){
                    //cout << "kir" << endl;
                    auto it = all_lines.begin();

                    dispatch_que.push_back(file_line1{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'F', it->dep_inst1, it->dep_inst2});
                    //////////////////////////////////////////////update fakerob
                    auto itm = fakerob.find(it->tag);
                    itm->second.fetch_t = cycle;

                    //all_lines.push_back(file_line1{line_separated.at(0), stoi(line_separated.at(1)), stoi(line_separated.at(2)),stoi(line_separated.at(3)), stoi(line_separated.at(4)),0, 'N' });
                    all_lines.erase(it);
                    it = all_lines.begin();
                } else {
                    break;
                }
            }



            cycle += 1;

        }
        int fetch_time;
        int fetch_cycles;
        int dispatch_time;
        int dispatch_cycles;
        int issue_time;
        int issue_cycles;
        int execute_time;
        int execute_cycles;
        int WB_time;
        int WB_cycles;
        for (auto it = fakerob.begin(); it != fakerob.end(); it++){
            fetch_time = it->second.fetch_t;
            fetch_cycles = 1;
            dispatch_time = fetch_time + 1;
            dispatch_cycles = it->second.disp_exit - dispatch_time;
            issue_time = it->second.disp_exit;
            issue_cycles = it->second.issue_exit - issue_time;
            execute_time = it->second.issue_exit;
            execute_cycles = it->second.exec_exit - execute_time;
            WB_time = it->second.exec_exit;
            WB_cycles = 1;

            cout << it->first << " fu{" << it->second.optype << "} " << "src{" << it->second.src1 << "," << it->second.src2 << "} dst{" << it->second.destreg << "} IF{" << fetch_time <<","<< 1 << "} ID{" << dispatch_time << "," << dispatch_cycles << "} IS{" << issue_time << "," << issue_cycles << "} EX{" << execute_time << "," << execute_cycles << "} WB{" << WB_time << "," << WB_cycles << "}" << endl;
        }
        cout << "number of instructions = " << fakerob.size() << endl;
        cout << "number of cycles       = " << cycle << endl;
        double IPC = ((double)fakerob.size()/(double)cycle);
        cout << fixed << showpoint << setprecision(5);
        cout << "IPC                    = " << IPC << endl;


        return 0;

    } else {


        map<int,char> RF;
        deque<file_line> all_lines;
        map<int,rob_struct> fakerob;


        //initializing RF
        for (int i=0; i<128; i++){
            RF[i] = 'y';
        }

        ifstream file (filename);
        string line;

        int mytag = 0;
        while (getline(file, line)){

            //cout << line<<endl;
            vector<string> line_separated;
            string line_element;
            istringstream s(line);
            while (getline(s, line_element, ' ')){
                line_separated.push_back(line_element);
            }
            all_lines.push_back(file_line{mytag,line_separated.at(0), stoi(line_separated.at(1)), stoi(line_separated.at(2)), stoi(line_separated.at(3)), stoi(line_separated.at(4)), 0, 'N' });
            fakerob.insert(pair<int,rob_struct>(mytag, rob_struct{stoi(line_separated.at(1)), stoi(line_separated.at(2)), stoi(line_separated.at(3)),stoi(line_separated.at(4)),0,0,0,0}));
            mytag += 1;
        }
        struct rob_struct{
            int optype;
            int destreg;
            int src1;
            int src2;
            int fetch_t;
            int disp_exit;
            int issue_exit;
            int exec_exit;
        };



        vector<file_line> fetch_que;
        vector<file_line> dispatch_que;
        vector<file_line> schedule_que;
        vector<file_line> execute_que;

        int available_FU;
        int available_schedule;
        int available_dispatch;
        int dispatch_bandwidth;


        int cycle = 0;
        while (!all_lines.empty() || !execute_que.empty() || !schedule_que.empty() || !dispatch_que.empty()){

            //cout << cycle << endl;

            ///////////processing execute_list
            //remove 'W's
            if (!execute_que.empty()){
                vector<file_line> v_temp;
                for (auto it = execute_que.begin(); it != execute_que.end(); it++){
                    if (it -> state == 'W' ) {
                        RF[it->destreg] = 'y';
                        v_temp.push_back(file_line{it->tag,it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'W'});
                    }
                }
                for (auto it=v_temp.begin(); it!=v_temp.end();it++){
                    execute_que.erase(remove(execute_que.begin(),execute_que.end(), file_line{it->tag,it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,it->state}), execute_que.end());
                }
            }




            //increment counters
            if (!execute_que.empty()){
                for (auto it = execute_que.begin(); it != execute_que.end(); it++){
                    it->counter = it->counter + 1;
                }
            }


            /////moving from execution to WB results
            //if counter == 0 or == 1 or ==2
            if (!execute_que.empty()){

                for (auto it = execute_que.begin(); it != execute_que.end(); it++){
                    if (it -> optype == 0){
                        if (it -> counter == 1){
                            /////// broadcast
                            it -> state = 'W';
                            /////////////////////////////////////////update fakerob
                            auto itm = fakerob.find(it->tag);
                            itm->second.exec_exit = cycle;
                            /// not broadcasting now
                            //if (it->destreg != -1){
                            //    RF[it->destreg] = 'y';
                            //}
                        }
                    }
                    if (it -> optype == 1){
                        if (it -> counter == 2){
                            /////// broadcast
                            it -> state = 'W';
                            //////////////////////////////////////////////update fakerob
                            auto itm = fakerob.find(it->tag);
                            itm->second.exec_exit = cycle;
                            /// not broadcasting now
                            //if (it->destreg != -1){
                            //    RF[it->destreg] = 'y';
                            //}
                        }
                    }
                    if (it -> optype == 2){
                        if (it -> counter == 5){
                            /////// broadcast
                            it -> state = 'W';
                            //////////////////////////////////////////////update fakerob
                            auto itm = fakerob.find(it->tag);
                            itm->second.exec_exit = cycle;
                            /// not broadcasting now
                            //if (it->destreg != -1){
                            //    RF[it->destreg] = 'y';
                            //}
                        }
                    }
                }
            }





            //available_FU = N + 1 - execute_que.size();
            available_FU = N + 1;
            /////from schedule to execute list (issuing)
            if (!schedule_que.empty()){
                vector<file_line> v_temp;
                for (auto it = schedule_que.begin(); it != schedule_que.end(); it++){
                    if (available_FU > 0) {
                        if ((it -> src1 != -1) && (it -> src2 != -1)){
                            if (RF[it -> src1] == 'y' && RF[it -> src2] == 'y'){
                                execute_que.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E'});
                                //////////////////////////////////////////////update fakerob
                                auto itm = fakerob.find(it->tag);
                                itm->second.issue_exit = cycle;

                                if (it -> destreg != -1){
                                    RF[it -> destreg] = 'n';
                                }
                                //schedule_que.erase(it);
                                v_temp.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S'});
                                available_FU -= 1;
                            }

                        } else if (it -> src1 != -1 && it -> src2 == -1){

                            if (RF[it -> src1] == 'y'){
                                execute_que.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E'});
                                //////////////////////////////////////////////update fakerob
                                auto itm = fakerob.find(it->tag);
                                itm->second.issue_exit = cycle;

                                if (it -> destreg != -1){
                                    RF[it -> destreg] = 'n';
                                }
                                //schedule_que.erase(it);
                                v_temp.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S'});
                                available_FU -= 1;
                            }

                        } else if ( it -> src1 == -1 && it -> src2 != -1){
                            if (RF[it -> src2] == 'y'){
                                execute_que.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E'});
                                //////////////////////////////////////////////update fakerob
                                auto itm = fakerob.find(it->tag);
                                itm->second.issue_exit = cycle;

                                if (it -> destreg != -1){
                                    RF[it -> destreg] = 'n';
                                }
                                //schedule_que.erase(it);
                                v_temp.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S'});
                                available_FU -= 1;
                            }

                        } else if (it -> src1 == -1 && it -> src2 == -1){
                            execute_que.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'E'});
                            //////////////////////////////////////////////update fakerob
                            auto itm = fakerob.find(it->tag);
                            itm->second.issue_exit = cycle;

                            if (it -> destreg != -1){
                                RF[it -> destreg] = 'n';
                            }
                            v_temp.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S'});
                            available_FU -= 1;

                        }
                    }
                }
                for (auto it=v_temp.begin(); it!=v_temp.end();it++){
                    schedule_que.erase(remove(schedule_que.begin(),schedule_que.end(), file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,it->state}), schedule_que.end());
                }

            }


            //////from Dispatch to Schedule list
            /*
            auto it = dispatch_que.begin();
            for (it = dispatch_que.begin(); it != dispatch_que.end(); it++){
                if (it->state == 'D'){
                    break;
                }
            }
            */
            available_schedule = S - schedule_que.size();
            dispatch_bandwidth = N;
            while (dispatch_bandwidth > 0){
                if (available_schedule > 0){
                    if (!dispatch_que.empty()){
                        auto itt = dispatch_que.begin();
                        if (itt->state == 'D'){
                            auto it = dispatch_que.begin();
                            schedule_que.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'S'});
                            //////////////////////////////////////////////update fakerob
                            auto itm = fakerob.find(it->tag);
                            itm->second.disp_exit = cycle;

                            //dest operand becomes ready
                            //RF[it -> destreg] = 'y';
                            dispatch_que.erase(it);
                            it = dispatch_que.begin();
                            available_schedule -= 1;
                            dispatch_bandwidth -= 1;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }



            ////Fetch state to Dispatch state
            if (!dispatch_que.empty()){

                for (auto it = dispatch_que.begin(); it != dispatch_que.end(); it++){
                    if (it->state == 'F'){
                        it->state = 'D';
                    }
                }
            }



            ////processing fetch list
            for (int i=0 ; i < N; i++){

                if (!all_lines.empty() && dispatch_que.size() < (2 * N)){
                    auto it = all_lines.begin();
                    ///////////////////////////////testing
                    if (it->src1 == -1 && it->src2 == -1 && it->destreg == -1){
                        //cout << it->tag << " " << it->optype << " " << it->destreg << " {" << it->src1 << "," << it->src2 << "} " << endl;
                        //kircount += 1;
                    }
                    //////////////////////////////testing
                    dispatch_que.push_back(file_line{it->tag, it->pcounter, it->optype, it->destreg,it->src1,it->src2,it->counter,'F'});
                    //////////////////////////////////////////////update fakerob
                    auto itm = fakerob.find(it->tag);
                    itm->second.fetch_t = cycle;

                    //all_lines.push_back(file_line{line_separated.at(0), stoi(line_separated.at(1)), stoi(line_separated.at(2)),stoi(line_separated.at(3)), stoi(line_separated.at(4)),0, 'N' });
                    all_lines.erase(it);
                    it = all_lines.begin();
                } else {
                    break;
                }
            }



            cycle += 1;
        }
        int fetch_time;
        int fetch_cycles;
        int dispatch_time;
        int dispatch_cycles;
        int issue_time;
        int issue_cycles;
        int execute_time;
        int execute_cycles;
        int WB_time;
        int WB_cycles;
        for (auto it = fakerob.begin(); it != fakerob.end(); it++){
            fetch_time = it->second.fetch_t;
            fetch_cycles = 1;
            dispatch_time = fetch_time + 1;
            dispatch_cycles = it->second.disp_exit - dispatch_time;
            issue_time = it->second.disp_exit;
            issue_cycles = it->second.issue_exit - issue_time;
            execute_time = it->second.issue_exit;
            execute_cycles = it->second.exec_exit - execute_time;
            WB_time = it->second.exec_exit;
            WB_cycles = 1;

            cout << it->first << " fu{" << it->second.optype << "} " << "src{" << it->second.src1 << "," << it->second.src2 << "} dst{" << it->second.destreg << "} IF{" << fetch_time <<","<< 1 << "} ID{" << dispatch_time << "," << dispatch_cycles << "} IS{" << issue_time << "," << issue_cycles << "} EX{" << execute_time << "," << execute_cycles << "} WB{" << WB_time << "," << WB_cycles << "}" << endl;
        }
        cout << "number of instructions = " << fakerob.size() << endl;
        cout << "number of cycles       = " << cycle << endl;
        double IPC = ((double)fakerob.size()/(double)cycle);
        cout << fixed << showpoint << setprecision(5);
        cout << "IPC                    = " << IPC << endl;

        return 0;

    }

}
