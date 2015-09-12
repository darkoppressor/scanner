/* Copyright (c) 2012 Cheese and Bacon Games, LLC */
/* This file is licensed under the MIT License. */
/* See the file docs/LICENSE.txt for the full license text. */

#ifndef file_io_h
#define file_io_h

#include <string>
#include <sstream>

#include <SDL.h>

#ifdef GAME_OS_ANDROID
    #include <vector>
    #include <stdint.h>
    #include <dirent.h>
    #include <sys/stat.h>
#else
    #include <boost/filesystem.hpp>
#endif

const std::string SAVE_TEMP_FILE_SUFFIX="_TEMP";

class File_IO_Load{
public:

    bool load_success;
    std::istringstream data;

    File_IO_Load();
    File_IO_Load(std::string path);

    void load_file(std::string path);

    bool file_loaded();
    bool eof();

    void getline(std::string* line);
    std::string get_data();
};

class File_IO_Binary_Load{
public:

    bool load_success;
    std::string data;

    File_IO_Binary_Load();
    File_IO_Binary_Load(std::string path);

    void load_file(std::string path);

    bool file_loaded();

    std::string get_data();
};

class File_IO_Binary_Save{
public:

    SDL_RWops* rwops;

    std::string path;

    File_IO_Binary_Save(std::string get_path);

    void open(std::string get_path);
    void close();

    bool is_opened();

    void write(const void* ptr,size_t data_size,size_t data_count);
};

#ifdef GAME_OS_ANDROID
    class File_IO{
    public:
        bool save_file(std::string path,std::string data,bool append=false,bool binary=false);
        //First saves to a temporary file, then renames that file to the final file
        bool save_important_file(std::string path,std::string data,bool append=false,bool binary=false);

        bool directory_exists(std::string path);
        bool file_exists(std::string path);
        bool is_directory(std::string path);
        bool is_regular_file(std::string path);
        void create_directory(std::string path);
        bool rename_file(std::string old_path,std::string new_path);
        //This does NOT overwrite new_path if it already exists
        void copy_file(std::string old_path,std::string new_path);
        void remove_file(std::string path);
        void remove_directory(std::string path);
        std::string get_file_name(std::string path);

        bool external_storage_available();
    };

    class File_IO_Directory_Iterator{
    public:

        std::string directory;
        std::vector<std::string> asset_list;
        uint32_t entry;

        File_IO_Directory_Iterator(std::string get_directory);

        bool evaluate();
        void iterate();
        bool is_directory();
        bool is_regular_file();
        std::string get_full_path();
        std::string get_file_name();
    };

    class File_IO_Directory_Iterator_User_Data{
    public:

        std::string directory;
        DIR* dir;
        struct dirent* dir_entry;
        uint32_t entry;
        uint32_t entries;

        File_IO_Directory_Iterator_User_Data(std::string get_directory);
        ~File_IO_Directory_Iterator_User_Data();

        bool evaluate();
        void iterate();
        bool file_exists();
        bool is_regular_file();
        bool is_directory();
        std::string get_full_path();
        std::string get_file_name();
    };
#else
    class File_IO{
    public:
        bool save_file(std::string path,std::string data,bool append=false,bool binary=false);
        //First saves to a temporary file, then renames that file to the final file
        bool save_important_file(std::string path,std::string data,bool append=false,bool binary=false);

        bool directory_exists(std::string path);
        bool file_exists(std::string path);
        bool is_directory(std::string path);
        bool is_regular_file(std::string path);
        void create_directory(std::string path);
        bool rename_file(std::string old_path,std::string new_path);
        //This does NOT overwrite new_path if it already exists
        void copy_file(std::string old_path,std::string new_path);
        void remove_file(std::string path);
        void remove_directory(std::string path);
        std::string get_file_name(std::string path);
    };

    class File_IO_Directory_Iterator{
    public:

        boost::filesystem::directory_iterator it;

        File_IO_Directory_Iterator(std::string get_directory);

        bool evaluate();
        void iterate();
        bool is_directory();
        bool is_regular_file();
        std::string get_full_path();
        std::string get_file_name();
    };
#endif

#endif
