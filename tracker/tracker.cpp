#include "tracker.h"

int read_user_group_file(map<int, group_data*>& group_data_map)
{
    int return_val = 0;
    int group_fd = open(GROUP_USER_FILE, O_RDONLY);
    if(group_fd > 0)
    {
        void *buffer  = (void *)new group_data[1];
        int read_bytes = -1;
        while((read_bytes = read(group_fd, buffer, sizeof(group_data))) > 0)
        {
            if(read_bytes == sizeof(group_data))
            {
                group_data *current_group = (group_data*)new group_data[1];
                memcpy(current_group, buffer, sizeof(group_data));
                group_data_map[current_group->group_id]=current_group;
            }
            else
            {
                printf("TRACKER::read_user_group::group info wrong\n");
                return_val = -1;
                break;
            }
        }
        delete []buffer;
        close(group_fd);
    }
    else
    {
        printf("TRACKER::read_user_group_file::failed to OPEN\n press any key");
        int cont = 0;
        scanf("%d", &cont);
        return_val = -2;
    }
    return return_val;
}

int read_user_data_file(map<int, user_data*> user_data_map)
{
    int return_val = 0;
    int user_fd = open(USER_DATA_FILE, O_RDONLY);
    if(user_fd > 0)
    {
      void *buffer  = (void *) new user_data[1];
        int read_bytes = -1;
        while((read_bytes = read(user_fd, buffer, sizeof(user_data))) > 0)
        {
            if(read_bytes == sizeof(user_data))
            {
                user_data *current_user = (user_data*)new user_data[1];
                memcpy(current_user, buffer, sizeof(user_data));
                user_data_map[current_user->user_id]=current_user;
            }
            else
            {
                printf("TRACKER::read_user_user::user info wrong\n");
                return_val = -1;
                break;
            }
        }
        delete[] buffer;
        close(user_fd);
    }
    else
    {
        printf("TRACKER::read_user_user_file::failed to OPEN\n press any key");
        int cont = 0;
        scanf("%d", &cont);
        return_val = -2;
    }
    return return_val;
}

int set_up(map<int, group_data*>& group_data_map, map<int, user_data*>& user_data_map)
{
    int status = 0;
    int status_group_file_read = read_user_group_file(group_data_map);
    int status_user_file_read = read_user_data_file(user_data_map);
    if(status_group_file_read==0 && status_user_file_read==0)
    {
        return status;
    }
    else
        status = -1;
    return status;
}

int end_phase(map<int, group_data*>& group_data_map, map<int, user_data*>& user_data_map)
{
    int return_status = 0;
    if(group_data_map.size())
    {
        int group_fd = open(GROUP_USER_FILE, O_CREAT|O_TRUNC|O_WRONLY, FILE_PERM);
        if(group_fd>0)
        {
            for(map<int, group_data*>::iterator i = group_data_map.begin(); i != group_data_map.end(); i++)
            {
                void *buff = (void *) new group_data[1];
                memcpy(buff, i->second, sizeof(group_data));
                int write_bytes = write(group_fd, buff, sizeof(group_data));
                if(write_bytes != sizeof(group_data))
                {
                    printf("end_phase::write bytes not tallying group data\n");
                    return_status = -1;
                    break;
                }
                delete[] buff;
            }
            close(group_fd);
        }
        else
        {
            return_status = -2;
            printf("end_phase::group file not opened\n");
        }
        
        if(return_status==0 && user_data_map.size())
        {
            int user_fd = open(USER_DATA_FILE, O_CREAT|O_TRUNC|O_WRONLY, FILE_PERM);
            if(user_fd)
            {
                for(map<int, user_data*>::iterator i = user_data_map.begin(); i != user_data_map.end(); i++)
                {
                    void *buff = (void*)new user_data[1];
                    memcpy(buff, i->second, sizeof(user_data));
                    int write_bytes = write(user_fd, buff, sizeof(user_data));
                    if(write_bytes != sizeof(user_data))
                    {
                        return_status = -3;
                        printf("endphase::write bytes not tallying user data\n");
                        break;
                    }
                    delete[] buff;
                }
                close(user_fd);
            }
            else
            {
                return_status = -4;  
                printf("end_phase::user_file failed to open\n");
            }   
        }
    }
    return return_status;
}