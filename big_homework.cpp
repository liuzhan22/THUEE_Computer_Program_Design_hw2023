#include <iostream>
#include <cstring>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "json.hpp"

using json = nlohmann::json;

// 全局变量
std::string userIdNow;
std::string userTypeNow;
std::string userNameNow;

// 需要提前声明的函数
void studentMenu( );
void menuFunctionStudent1( );
void menuFunctionStudent2( );
void menuFunctionStudent3( );
void menuFunctionStudent4( );
void menuFunctionTeacher1( );
void menuFunctionTeacher2( );
void menuFunctionTeacher3( );
void mainMenu( );
void teacherMenu( );
void clearScreen( );

// 百分制与绩点转换函数
double convertToGPA(double percentage) {
    if( percentage >= 98 && percentage <= 100 ) {
        return 4.0; // A+
    }
    else if( percentage >= 92 && percentage < 98 ) {
        return 4.0; // A
    }
    else if( percentage >= 90 && percentage < 92 ) {
        return 4.0; // A-
    }
    else if( percentage >= 87 && percentage < 90 ) {
        return 3.6; // B+
    }
    else if( percentage >= 82 && percentage < 87 ) {
        return 3.3; // B
    }
    else if( percentage >= 80 && percentage < 82 ) {
        return 3.0; // B-
    }
    else if( percentage >= 78 && percentage < 80 ) {
        return 3.0; // B-
    }
    else if( percentage >= 75 && percentage < 78 ) {
        return 2.6; // C+
    }
    else if( percentage >= 71 && percentage < 75 ) {
        return 2.3; // C
    }
    else if( percentage >= 68 && percentage < 71 ) {
        return 2.0; // C-
    }
    else if( percentage >= 65 && percentage < 68 ) {
        return 1.6; // D+
    }
    else if( percentage >= 61 && percentage < 65 ) {
        return 1.3; // D
    }
    else {
        return 0.0; // F
    }
}

// object类
class object
{
public:
    virtual void set( ) = 0;
    virtual void show( ) = 0;
    std::string id;
    std::string name;
};

// people类
class people : public object
{
public:
    void set( ) override;
    void show( ) override;

private:
};

// people类的set函数override
void people::set( )
{
    std::cout << "请输入学工号：\n";
    std::cin >> id;
    std::cout << "请输入姓名：\n";
    std::cin >> name;
}

// people类的show函数override
void people::show( )
{
    std::cout << "学工号：" << id << std::endl;
    std::cout << "姓名：" << name << std::endl;
}

// 简化版course类，用于在student类下储存数据
class courseStoredStudent
{
public:
    std::string id;
    std::string name;
    double courseCredit;
    double grade = 0;
    double GPA = 0;
};

// 简化版学生类，用于course类下vector容器储存数据
class studentOfCourse
{
public:
    std::string id;
    std::string name;
    double grade = 0;
};

// course类
class course : public object
{
public:
    void set( ) override;
    void show( ) override;
    double courseCredit;
    std::string teacherName;
    std::vector<studentOfCourse> courseStudents;
    json toJson( ) const;
    void fromJson(const json& j);
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    void loadFromFileFixedId(const std::string& filename, std::string fixedId);

protected:
    double studentAverageScore;
};

// 将course对象转换为json对象
json course::toJson( ) const
{
    json j;
    j["id"] = id;
    j["name"] = name;
    j["teacher"] = teacherName;
    j["credit"] = courseCredit;

    // 将选择此课程学生信息转换为 JSON 数组
    json studentsJson = json::array( );
    for( const auto& courseChoosingStudent : courseStudents )
    {
        json studentJson;
        studentJson["id"] = courseChoosingStudent.id;
        studentJson["name"] = courseChoosingStudent.name;
        studentJson["grade"] = courseChoosingStudent.grade;
        studentsJson.push_back(studentJson);
    }
    j["students"] = studentsJson;

    return j;
}

// 将json对象转换为course对象
void course::fromJson(const json& j)
{
    id = j["id"].get<std::string>( );
    name = j["name"].get<std::string>( );
    teacherName = j["teacher"].get<std::string>( );
    courseCredit = j["credit"].get<double>( );

    courseStudents.clear( );
    for( const auto& studentJson : j["students"] )
    {
        studentOfCourse aStudent;
        aStudent.id = studentJson["id"].get<std::string>( );
        aStudent.name = studentJson["name"].get<std::string>( );
        aStudent.grade = studentJson["grade"].get<double>( );
        courseStudents.push_back(aStudent);
    }
}

// 将course对象存为json文件
void course::saveToFile(const std::string& filename) const
{
    json jsonCourseArray = json::array( ); // 最终存入的是一个json数组
    json jsonCourseData;                  // 此时不能确认文件中读入的jsonCourseData是json对象还是json数组
    std::ifstream fileIn(filename);
    if( fileIn.is_open( ) )
    {
        fileIn >> jsonCourseData;
        fileIn.close( );
    }

    bool foundCourse = false;

    if( jsonCourseData.is_array( ) )
    {
        for( auto& courseData : jsonCourseData )
        {
            if( courseData["id"].get<std::string>( ) == id )
            {
                courseData = toJson( );
                foundCourse = true;
                jsonCourseArray = jsonCourseData;
                break;
            }
        }

        if( !foundCourse )
        {
            json newCourseData = toJson( );
            jsonCourseData.push_back(newCourseData);
            jsonCourseArray = jsonCourseData;
        }
    }
    else if( jsonCourseData.is_object( ) )
    {
        if( jsonCourseArray["id"].get<std::string>( ) == id )
        {
            jsonCourseData = toJson( );
            jsonCourseArray.push_back(jsonCourseData);
        }
        else if( jsonCourseArray["id"].get<std::string>( ) != id )
        {
            json newCourseData = toJson( );
            jsonCourseArray.push_back(jsonCourseData);
            jsonCourseArray.push_back(newCourseData);
        }
    }

    std::ofstream fileOut(filename);
    if( fileOut.is_open( ) )
    {
        fileOut << jsonCourseArray.dump(4);
        fileOut.close( );
        //std::cout << "数据已保存至" << filename << std::endl;
    }
    else
    {
        std::cout << "无法保存数据至 " << filename << std::endl;
    }
}

// 从json文件中加载符合当前id的course对象
void course::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if( file.is_open( ) )
    {
        json jsonCourseArray;
        file >> jsonCourseArray;
        file.close( );

        if( jsonCourseArray.is_array( ) )
        {
            for( const auto& courseData : jsonCourseArray )
            {
                if( courseData["id"].get<std::string>( ) == id )
                {
                    fromJson(courseData);
                    // show( );
                    return;
                }
            }
        }
        else if( jsonCourseArray.is_object( ) )
        {
            if( jsonCourseArray["id"].get<std::string>( ) == id ) // 有bug：如果jsonCourseArray是空json对象，则会数组越界
            {
                fromJson(jsonCourseArray);
                // show( );
                return;
            }
            else if( jsonCourseArray["id"].get<std::string>( ) != id )
            {
                return;
            }
        }
        //std::cout << "数据已从" << filename << " 加载。" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "创建新文件 " << filename << " 成功！" << std::endl;
        }
        else
        {
            std::cout << "无法创建新文件 " << filename << std::endl;
        }
    }

    //std::cout << "未找到符合当前id的course对象数据。" << std::endl;
}

// 从json文件中加载符合当前id的course对象，需要指定要加载的课程id
void course::loadFromFileFixedId(const std::string& filename, std::string fixedId)
{
    std::ifstream file(filename);
    if( file.is_open( ) )
    {
        json jsonCourseArray;
        file >> jsonCourseArray;
        file.close( );

        if( jsonCourseArray.is_array( ) )
        {
            for( const auto& courseData : jsonCourseArray )
            {
                if( courseData["id"].get<std::string>( ) == fixedId )
                {
                    fromJson(courseData);
                    // show( );
                    return;
                }
            }
        }
        else if( jsonCourseArray.is_object( ) )
        {
            if( jsonCourseArray["id"].get<std::string>( ) == fixedId ) // 有bug：如果jsonCourseArray是空json对象，则会数组越界
            {
                fromJson(jsonCourseArray);
                // show( );
                return;
            }
            else if( jsonCourseArray["id"].get<std::string>( ) != fixedId )
            {
                return;
            }
        }
        //std::cout << "数据已从" << filename << " 加载。" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "创建新文件 " << filename << " 成功！" << std::endl;
        }
        else
        {
            std::cout << "无法创建新文件 " << filename << std::endl;
        }
    }

    //std::cout << "未找到符合当前id的course对象数据。" << std::endl;
}

// 简化版course类，用于在teacher类下储存数据
class courseStoredTeacher
{
public:
    std::string id;
    std::string name;
    double courseCredit;
    double averageGrade = 0;
    double averageGPA = 0;
};

// course的set函数override
void course::set( )
{
    std::cout << "请输入学生成绩：\n";
}

// course的show函数override
void course::show( )
{
    std::cout << "展示学生成绩：\n";
}

// student类
class student : public people
{
public:
    student( );
    void chooseCourse( );
    void quitCourse( );
    void checkGrade( );
    void calculateGPA( );
    json toJson( ) const;                                // 将学生对象转换为 JSON 对象
    void fromJson(const json& j);                       // 从 JSON 对象恢复学生数据
    void saveToFile(const std::string& filename) const; // 将学生对象保存到 JSON 文件
    void loadFromFile(const std::string& filename);     // 从 JSON 文件加载学生数据
    void loadFromFileFixedId(const std::string& filename, std::string fixId);
    friend void menuFunctionTeacher2( );
private:
    std::vector<courseStoredStudent> studentCourses;
};

// student类构造函数：读取全局变量id和name
student::student( )
{
    id = userIdNow;
    name = userNameNow;
    // std::cout << "当前账号为：" << userIdNow << ",欢迎 " << userNameNow << "!" << std::endl;
}

// student选课函数
void student::chooseCourse( )
{
    // 加载被选课程
    courseStoredStudent chooseACourse;
    std::cout << "请输入选择课程课序号：\n";
    std::cin >> chooseACourse.id;
    course chosedCourse;
    chosedCourse.loadFromFileFixedId("course.json", chooseACourse.id);

    loadFromFile("student.json");

    // 检查选没选过该课程
    for( const auto& course : studentCourses )
    {
        if( course.id == chooseACourse.id )
        {
            std::cout << "你已选过该课程！\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
    }

    // 选课信息写入student -> <vector>courseStoredStudent
    chooseACourse.name = chosedCourse.name;
    chooseACourse.courseCredit = chosedCourse.courseCredit;
    studentCourses.push_back(chooseACourse);
    saveToFile("student.json");

    // 选课信息写入course -> <vector>courseStudents
    studentOfCourse aNewStudent;
    aNewStudent.id = id;
    aNewStudent.name = name;
    chosedCourse.courseStudents.push_back(aNewStudent);
    chosedCourse.saveToFile("course.json");
    std::cout << "已选择课程" << chosedCourse.name << std::endl;
    std::cout << "(3秒后自动返回)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

// student退课函数
void student::quitCourse( )
{
    // 加载退课课程
    courseStoredStudent quitACourse;
    std::cout << "请输入退课课程课程号：\n";
    std::cin >> quitACourse.id;
    course quitedCourse;
    quitedCourse.loadFromFileFixedId("course.json", quitACourse.id);

    // 退课信息写入student -> <vector>courseStoredStudent
    loadFromFile("student.json");

    // 删除studentCourses容器中id为quitedCourse.id的对象
    studentCourses.erase(
        std::remove_if(studentCourses.begin( ), studentCourses.end( ),
            [quitACourse] (const courseStoredStudent& course)
            {
                return course.id == quitACourse.id;
            }),
        studentCourses.end( ));

    saveToFile("student.json");

    // 退课信息写入course -> <vector>courseStudents
    studentOfCourse aGoStudent;
    aGoStudent.id = id;

    // 删除courseStudents容器中此id的学生
    quitedCourse.courseStudents.erase(
        std::remove_if(quitedCourse.courseStudents.begin( ), quitedCourse.courseStudents.end( ),
            [this] (const studentOfCourse& student)
            {
                return student.id == id;
            }),
        quitedCourse.courseStudents.end( ));

    quitedCourse.saveToFile("course.json");
    std::cout << "已退出课程" << quitedCourse.name << std::endl;
    std::cout << "(3秒后自动返回)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

// 将student对象转为json对象
json student::toJson( ) const
{
    json j;
    j["id"] = id;
    j["name"] = name;

    // 将学生选课信息转换为 JSON 数组
    json coursesJson = json::array( );
    for( const auto& courseStoredStudent : studentCourses )
    {
        json courseJson;
        courseJson["id"] = courseStoredStudent.id;
        courseJson["name"] = courseStoredStudent.name;
        courseJson["credit"] = courseStoredStudent.courseCredit;
        courseJson["grade"] = courseStoredStudent.grade;
        courseJson["GPA"] = courseStoredStudent.GPA;
        coursesJson.push_back(courseJson);
    }
    j["courses"] = coursesJson;

    return j;
}

// 将json对象转为student对象
void student::fromJson(const json& j)
{
    id = j["id"].get<std::string>( );
    name = j["name"].get<std::string>( );

    // 从 JSON 数组恢复学生选课信息
    studentCourses.clear( );
    for( const auto& courseJson : j["courses"] )
    {
        courseStoredStudent course;
        course.id = courseJson["id"].get<std::string>( );
        course.name = courseJson["name"].get<std::string>( );
        course.courseCredit = courseJson["credit"].get<double>( );
        course.grade = courseJson["grade"].get<double>( );
        course.GPA = courseJson["GPA"].get<double>( );
        studentCourses.push_back(course);
    }
}

// 将student对象存为json文件
void student::saveToFile(const std::string& filename) const
{
    std::ifstream fileIn(filename);
    json jsonStudentData;
    json jsonStudentArray = json::array( );
    if( fileIn.is_open( ) )
    {
        fileIn >> jsonStudentData;
        fileIn.close( );
    }

    bool foundStudent = false;

    if( jsonStudentData.is_array( ) )
    {
        for( auto& studentData : jsonStudentData )
        {
            if( studentData["id"].get<std::string>( ) == id )
            {
                studentData = toJson( );
                foundStudent = true;
                jsonStudentArray = jsonStudentData;
                break;
            }
        }

        if( !foundStudent )
        {
            json newStudentData = toJson( );
            jsonStudentData.push_back(newStudentData);
            jsonStudentArray = jsonStudentData;
        }
    }
    else if( jsonStudentData.is_object( ) )
    {
        // std::cout << "student saveToFile is_object\n";
        if( jsonStudentData["id"].get<std::string>( ) == id )
        {
            jsonStudentData = toJson( );
            jsonStudentArray.push_back(jsonStudentData);
        }
        else if( jsonStudentData["id"].get<std::string>( ) != id )
        {
            json newStudentData = toJson( );
            jsonStudentArray.push_back(jsonStudentData);
            jsonStudentArray.push_back(newStudentData);
        }
    }

    std::ofstream fileOut(filename);
    if( fileOut.is_open( ) )
    {
        fileOut << jsonStudentArray.dump(4);
        fileOut.close( );
        //std::cout << "数据已保存至" << filename << std::endl;
    }
    else
    {
        std::cout << "无法保存数据至 " << filename << std::endl;
    }
}

// 从json文件中加载符合当前id的student对象
void student::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if( file.is_open( ) )
    {
        // std::cout << "studentLoadFromFile is successfully open\n";
        json jsonStudentArray;
        file >> jsonStudentArray;
        file.close( );
        if( jsonStudentArray.is_array( ) )
        {
            // std::cout << "studentLoadFromFile jsonStudentArray is a array\n";
            for( const auto& studentData : jsonStudentArray )
            {
                student stu;
                stu.id = studentData["id"].get<std::string>( );
                if( id == stu.id )
                {
                    fromJson(studentData);
                    break;
                }
            }
        }
        else if( jsonStudentArray.is_object( ) )
        {
            // std::cout << "studentLoadFromFile jsonStudentArray is a object\n";
            if( id == jsonStudentArray["id"].get<std::string>( ) ) // 有bug：如果jsonStudentArray是空json对象，则会数组越界
            {
                fromJson(jsonStudentArray);
            }
            else if( id != jsonStudentArray["id"].get<std::string>( ) )
            {
                return;
            }
        }
        //std::cout << "数据已从" << filename << " 加载。" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "创建新文件 " << filename << " 成功！" << std::endl;
        }
        else
        {
            std::cout << "无法创建新文件 " << filename << std::endl;
        }
    }
}

// 从json文件中加载符合当前id的student对象，需提供student的id
void student::loadFromFileFixedId(const std::string& filename, std::string fixId)
{
    std::ifstream file(filename);
    if( file.is_open( ) )
    {
        // std::cout << "studentLoadFromFile is successfully open\n";
        json jsonStudentArray;
        file >> jsonStudentArray;
        file.close( );
        if( jsonStudentArray.is_array( ) )
        {
            // std::cout << "studentLoadFromFile jsonStudentArray is a array\n";
            for( const auto& studentData : jsonStudentArray )
            {
                student stu;
                stu.id = studentData["id"].get<std::string>( );
                if( fixId == stu.id )
                {
                    fromJson(studentData);
                    break;
                }
            }
        }
        else if( jsonStudentArray.is_object( ) )
        {
            // std::cout << "studentLoadFromFile jsonStudentArray is a object\n";
            if( fixId == jsonStudentArray["id"].get<std::string>( ) ) // 有bug：如果jsonStudentArray是空json对象，则会数组越界
            {
                fromJson(jsonStudentArray);
            }
            else if( fixId != jsonStudentArray["id"].get<std::string>( ) )
            {
                return;
            }
        }
        //std::cout << "数据已从" << filename << " 加载。" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "创建新文件 " << filename << " 成功！" << std::endl;
        }
        else
        {
            std::cout << "无法创建新文件 " << filename << std::endl;
        }
    }
}

// 学生查分的函数
void student::checkGrade( )
{
    std::string courseId;
    std::cout << "请输入要查分的课程ID：";
    std::cin >> courseId;

    loadFromFile("student.json");

    bool found = false;
    for( const auto& course : studentCourses )
    {
        if( course.id == courseId )
        {
            std::cout << "课程ID: " << course.id << std::endl;
            std::cout << "课程名称: " << course.name << std::endl;
            std::cout << "课程成绩: " << course.grade << std::endl;
            std::cout << "课程绩点: " << std::fixed << std::setprecision(1) << course.GPA << std::endl;
            std::cout << "(3秒后自动返回学生主菜单)\n";
            found = true;
            break;
        }
    }

    if( !found )
    {
        std::cout << "未找到该课程ID的成绩信息" << std::endl;
    }
}

// 学生计算均绩
void student::calculateGPA( )
{
    loadFromFile("student.json");
    double totalPoint = 0;
    double totalCredit = 0;
    for( const auto& course : studentCourses )
    {
        totalCredit += course.courseCredit;
        totalPoint += course.courseCredit * course.GPA;
    }
    double averageGPA;
    averageGPA = totalPoint / totalCredit;
    std::cout << "你好，" << name << "!\n";
    std::cout << "你的平均学分绩为：\n";
    std::cout << std::fixed << std::setprecision(3) << averageGPA << std::endl;
}

// teacher类
class teacher : public people
{
public:
    teacher( );
    void startCourse( );
    void showCourseGrade( );
    void uploadGrade( );
    json toJson( ) const;
    void fromJson(const json& j);
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    friend void menuFunctionTeacher2( );

private:
    std::vector<courseStoredTeacher> teacherCourses;
};

// teacher类构造函数：读取全局变量id和name
teacher::teacher( )
{
    id = userIdNow;
    name = userNameNow;
    // std::cout << "当前账号为：" << userIdNow << ",欢迎 " << userNameNow << "!" << std::endl;
}

// 教师开设课程
void teacher::startCourse( )
{
    // 存入teacher -> <vector>course下
    courseStoredTeacher startACourse;
    std::cout << "请输入开设课程课序号：\n";
    std::cin >> startACourse.id;
    std::cout << "请输入开设课程名称：\n";
    std::cin >> startACourse.name;
    std::cout << "请输入课程学分：\n";
    std::cin >> startACourse.courseCredit;
    teacherCourses.push_back(startACourse);

    // 存入course.json
    course newCourse;
    newCourse.id = startACourse.id;
    newCourse.name = startACourse.name;
    newCourse.teacherName = name;
    newCourse.courseCredit = startACourse.courseCredit;
    newCourse.saveToFile("course.json");
}

// 将teacher对象转为json对象
json teacher::toJson( ) const
{
    json j;
    j["id"] = id;
    j["name"] = name;

    // 将教师的课程信息转换为 JSON 数组
    json coursesJson = json::array( );
    for( const auto& courseStoredTeacher : teacherCourses )
    {
        json courseJson;
        courseJson["id"] = courseStoredTeacher.id;
        courseJson["name"] = courseStoredTeacher.name;
        courseJson["credit"] = courseStoredTeacher.courseCredit;
        coursesJson.push_back(courseJson);
    }
    j["courses"] = coursesJson;

    return j;
}

// 将json对象转为teacher对象
void teacher::fromJson(const json& j)
{
    id = j["id"].get<std::string>( );
    name = j["name"].get<std::string>( );

    // 从 JSON 数组恢复教师的课程信息
    teacherCourses.clear( );
    for( const auto& courseJson : j["courses"] )
    {
        courseStoredTeacher course;
        course.id = courseJson["id"].get<std::string>( );
        course.name = courseJson["name"].get<std::string>( );
        course.courseCredit = courseJson["credit"].get<double>( );
        teacherCourses.push_back(course);
    }
}

// 将teacher对象存为json文件
void teacher::saveToFile(const std::string& filename) const
{
    std::ifstream fileIn(filename);
    json jsonTeacherData;
    json jsonTeacherArray = json::array( );
    if( fileIn.is_open( ) )
    {
        fileIn >> jsonTeacherData;
        fileIn.close( );
        // std::cout << "teacher saveToFile successfully!\n";
    }
    // else
    //{
    //     jsonTeacherArray = json::array( );
    // }

    bool foundTeacher = false;

    if( jsonTeacherData.is_array( ) )
    {
        for( auto& teacherData : jsonTeacherData )
        {
            if( teacherData["id"].get<std::string>( ) == id )
            {
                teacherData = toJson( );
                foundTeacher = true;
                jsonTeacherArray = jsonTeacherData;
                // std::cout << "teacher saveToFile find teacher successfully!\n";
                // std::cout << "id:" << teacherData["id"].get<std::string>( ) << std::endl;
                break;
            }
        }

        if( !foundTeacher )
        {
            json newTeacherData = toJson( );
            jsonTeacherData.push_back(newTeacherData);
            jsonTeacherArray = jsonTeacherData;
        }
    }
    else if( jsonTeacherData.is_object( ) )
    {
        if( jsonTeacherData["id"].get<std::string>( ) == id )
        {
            jsonTeacherData = toJson( );
            jsonTeacherArray.push_back(jsonTeacherData);
        }
        else if( jsonTeacherData["id"].get<std::string>( ) != id )
        {
            json newTeacherData = toJson( );
            jsonTeacherArray.push_back(jsonTeacherData);
            jsonTeacherArray.push_back(newTeacherData);
        }
    }

    std::ofstream fileOut(filename);
    if( fileOut.is_open( ) )
    {
        fileOut << jsonTeacherArray.dump(4);
        fileOut.close( );
        //std::cout << "数据已保存至" << filename << std::endl;
    }
    else
    {
        std::cout << "无法保存数据" << filename << std::endl;
    }
}

// 从json文件中加载符合当前id的teacher对象
void teacher::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if( file.is_open( ) )
    {
        // std::cout << "teacher loadFromFile file open successfully!\n";
        json jsonTeacherArray;
        file >> jsonTeacherArray;
        file.close( );
        if( jsonTeacherArray.is_array( ) )
        {
            for( const auto& teacherData : jsonTeacherArray )
            {
                teacher tea;
                tea.id = teacherData["id"].get<std::string>( );
                if( id == tea.id )
                {
                    fromJson(teacherData);
                    // show( );
                    break;
                }
            }
        }
        else if( jsonTeacherArray.is_object( ) ) // 有bug：如果jsonTeacherArray是空json对象，则会数组越界
        {
            if( id == jsonTeacherArray["id"].get<std::string>( ) )
            {
                fromJson(jsonTeacherArray);
            }
            else if( id != jsonTeacherArray["id"].get<std::string>( ) )
            {
                return;
            }
        }
        //std::cout << "数据已从 " << filename << " 加载。" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "创建新文件 " << filename << " 成功！" << std::endl;
        }
        else
        {
            std::cout << "无法创建新文件 " << filename << std::endl;
        }
    }
}

// 检查用户名是否被占用
bool isUsernameTaken(const std::string& username)
{
    std::ifstream file("users.txt");
    std::string storedUsername, storedPassword;

    while( file >> storedUsername >> storedPassword )
    {
        if( storedUsername == username )
        {
            file.close( );
            return true; // 用户名已存在
        }
    }

    file.close( );
    return false; // 用户名未被占用
}

// 用户注册使用函数
void registerUser( )
{
    std::string userId, password, passwordAgain, role, roletag, userName;

    // 检查学工号是否被注册
    std::ifstream fileCheck("users.txt");
    if( fileCheck.is_open( ) )
    {
        std::string storedUserId, storedPassword;
        do
        {
            std::cout << "请输入学工号：";
            std::cin >> userId;

            if( isUsernameTaken(userId) )
            {
                std::cout << "学工号已被占用，请重新输入：" << std::endl;
            }
        } while( isUsernameTaken(userId) );
    }

    std::cout << "请设置密码：";
    std::cin >> password;
    std::cout << "请再次输入密码：";
    std::cin >> passwordAgain;

    // 两次密码不同时，要求重新输入
    while( password != passwordAgain )
    {
        std::cout << "密码错误！请确保两次密码相同！" << std::endl;
        std::cout << "请设置密码：";
        std::cin >> password;
        std::cout << "请再次输入密码：";
        std::cin >> passwordAgain;
    }
    std::cout << "请输入用户名：\n";
    std::cin >> userName;
    std::cout << "请问您是教师还是学生？\n教师选t，学生选s：\n";
    std::cin >> roletag;
    if( roletag == "t" )
        role = "teacher";
    else if( roletag == "s" )
        role = "student";
    // 打开文件以保存用户信息
    std::ofstream file("users.txt", std::ios::app);
    if( file.is_open( ) )
    {
        file << userId << " " << password << " "
            << " " << userName << " " << role << std::endl;
        file.close( );
        std::cout << "注册成功！" << std::endl << "(3秒后返回)\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        clearScreen( );
        mainMenu( );
    }
    else
    {
        std::cout << "Error opening file." << std::endl;
    }
}

// 登录检查用户名和密码函数
void loginUser( )
{
    std::string userId, password;
    std::cout << "请输入学工号：";
    std::cin >> userId;

    // 打开文件以检查用户信息
    std::ifstream file("users.txt");
    if( file.is_open( ) )
    {
        std::string storedUserId, storedPassword, userName, userType;
        file.seekg(0, std::ios::beg);
        while( file >> storedUserId >> storedPassword >> userName >> userType )
        {
            if( storedUserId == userId )
            {
                std::cout << "请输入密码：";
                std::cin >> password;
                while( password != storedPassword )
                {
                    std::cout << "密码错误，请重新输入：";
                    std::cin >> password;
                }
                userIdNow = userId;
                userTypeNow = userType;
                userNameNow = userName;
                std::cout << "登陆成功！\n";
                file.close( );
                return;
            }
        }
    }
    std::cout << "用户不存在！" << std::endl;
    file.close( );
}

// 函数用于居中输出文本
void printCentered(const std::string& text, int width)
{
    int padding = ( width - text.length( ) ) / 2;
    std::cout << std::setw(padding + text.length( )) << std::setfill(' ') << text << std::endl;
}

// 函数用于输出菜单项
void printMenuItem(const std::string& text, int width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << text;
}

// 清空控制台输出的函数
void clearScreen( )
{
#ifdef _WIN32 // Windows 系统
    system("cls");
#elif defined(__linux__) || defined(__APPLE__) // Linux 或 macOS 系统
    system("clear");
#else
    // 其他操作系统，无法自动识别
    // 可以给出一个错误提示或使用默认的清空屏幕方法
    std::cout << "无法自动识别操作系统，请手动清空屏幕。" << std::endl;
#endif
}

// 登录主界面
void mainMenu( )
{
    /*登录主界面*/
    int loginRegisterChoice;
    /*绘制菜单工作栏*/

    // 定义菜单标题和菜单项
    std::string title = "欢迎来到学生成绩管理系统！";
    std::string tip = "请输入您的选项序号：";
    std::string menuItem1 = "1. 师生登录";
    std::string menuItem2 = "2. 师生注册";
    std::string menuItem3 = "3. 退出系统";

    // 定义界面宽度和边框字符
    int width = 30;
    char borderChar = '*';

    // 输出界面顶部边框
    std::cout << std::string(width, borderChar) << std::endl;

    // 输出标题
    printCentered(title, width);

    // 输出菜单项
    std::cout << std::string(width, borderChar) << std::endl;
    printCentered(menuItem1, width);
    printCentered(menuItem2, width);
    printCentered(menuItem3, width);

    // 输出界面底部边框
    std::cout << std::string(width, borderChar) << std::endl;

    /****************/

    std::cin >> loginRegisterChoice;
    while( loginRegisterChoice != 1 && loginRegisterChoice != 2 && loginRegisterChoice != 3 )
    {
        std::cout << "无效选项，请重新输入：" << std::endl;
        std::cin >> loginRegisterChoice;
    }
    if( loginRegisterChoice == 1 )
    {
        clearScreen( );
        std::cout << "主界面 >> 登录\n";
        loginUser( );
        if( userTypeNow == "teacher" )
        {
            clearScreen( );
            std::cout << "主界面 >> 登录 >> 教师界面" << std::endl;
            teacherMenu( );
        }
        else if( userTypeNow == "student" )
        {
            clearScreen( );
            std::cout << "主界面 >> 登录 >> 学生界面" << std::endl;
            studentMenu( );
        }
    }
    else if( loginRegisterChoice == 2 )
    {
        clearScreen( );
        std::cout << "主界面 >> 注册\n";
        registerUser( );
    }
    else if( loginRegisterChoice == 3 )
    {
        clearScreen( );
        std::cout << "程序已退出\n";
        std::cout << "谢谢使用，再见！\n";
        std::exit(1);
    }
    /************/
}

// 学生界面菜单操作
void studentMenu( )
{
    /*绘制菜单工作栏*/

    // 定义菜单标题和菜单项
    std::string title = "学生成绩管理系统——学生端";
    std::string menuItem1 = "1. 选择课程";
    std::string menuItem2 = "2. 退出课程";
    std::string menuItem3 = "3. 课程查分";
    std::string menuItem4 = "4. 查询均绩";
    std::string menuItem5 = "5. 退出系统";

    // 定义界面宽度和边框字符
    int width = 30;
    char borderChar = '*';

    // 输出界面顶部边框
    std::cout << std::endl;
    std::cout << std::string(width, borderChar) << std::endl;

    // 输出标题
    printCentered(title, width);

    // 输出菜单项
    std::cout << std::string(width, borderChar) << std::endl;
    printCentered(menuItem1, width);
    printCentered(menuItem2, width);
    printCentered(menuItem3, width);
    printCentered(menuItem4, width);
    printCentered(menuItem5, width);
    std::cout << std::endl;

    // 输出界面底部边框
    std::cout << std::string(width, borderChar) << std::endl;

    /****************/

    /*功能选择*/
    std::cout << std::endl;
    std::cout << "请输入需要进行的操作：" << std::endl;
    std::cout << "(输入对应操作的序号即可)" << std::endl;
    int menuItemChoice;
    std::cin >> menuItemChoice;
    switch( menuItemChoice )
    {
    case 1:
        clearScreen( );
        std::cout << "已进入功能1：选择课程" << std::endl;
        menuFunctionStudent1( );
        break;
    case 2:
        clearScreen( );
        std::cout << "已进入功能2：退出课程" << std::endl;
        menuFunctionStudent2( );
        break;
    case 3:
        clearScreen( );
        std::cout << "已进入功能3：课程查分" << std::endl;
        menuFunctionStudent3( );
        break;
    case 4:
        clearScreen( );
        std::cout << "已进入功能4：查询均绩" << std::endl;
        menuFunctionStudent4( );
        break;
    case 5:
        clearScreen( );
        std::cout << "程序已退出\n";
        std::cout << "谢谢使用，再见！\n";
        std::exit(1);
    default:
        std::cout << "输入选项无效，请重新选择：" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        clearScreen( );
        studentMenu( );
        break;
    }
    /**********/
}

// 教师界面菜单操作
void teacherMenu( )
{
    /*绘制菜单工作栏*/

    // 定义菜单标题和菜单项
    std::string title = "学生成绩管理系统——教师端";
    std::string menuItem1 = "1. 开设课程";
    std::string menuItem2 = "2. 课程打分";
    std::string menuItem3 = "3. 课程查分";
    std::string menuItem4 = "4. 退出系统";

    // 定义界面宽度和边框字符
    int width = 30;
    char borderChar = '*';

    // 输出界面顶部边框
    std::cout << std::endl;
    std::cout << std::string(width, borderChar) << std::endl;

    // 输出标题
    printCentered(title, width);

    // 输出菜单项
    std::cout << std::string(width, borderChar) << std::endl;
    printCentered(menuItem1, width);
    printCentered(menuItem2, width);
    printCentered(menuItem3, width);
    printCentered(menuItem4, width);
    std::cout << std::endl;

    // 输出界面底部边框
    std::cout << std::string(width, borderChar) << std::endl;

    /****************/

    /*功能选择*/
    std::cout << std::endl;
    std::cout << "请输入需要进行的操作：" << std::endl;
    std::cout << "(输入对应操作的序号即可)" << std::endl;
    int menuItemChoice;
    std::cin >> menuItemChoice;
    switch( menuItemChoice )
    {
    case 1:
        clearScreen( );
        std::cout << "已进入功能1：开设课程" << std::endl;
        menuFunctionTeacher1( );
        break;
    case 2:
        clearScreen( );
        std::cout << "已进入课程2：课程打分" << std::endl;
        menuFunctionTeacher2( );
        break;
    case 3:
        clearScreen( );
        std::cout << "已进入课程3：课程查分" << std::endl;
        menuFunctionTeacher3( );
        break;
    case 4:
        clearScreen( );
        std::cout << "程序已退出\n";
        std::cout << "谢谢使用，再见！\n";
        std::exit(1);
    default:
        std::cout << "输入选项无效，请重新选择：" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        clearScreen( );
        teacherMenu( );
        break;
    }
    /**********/
}

// 教师功能1：开设课程
void menuFunctionTeacher1( )
{
    teacher tea;
    tea.loadFromFile("teacher.json");
    tea.startCourse( );
    tea.saveToFile("teacher.json");
}

// 教师功能2：课程打分
void menuFunctionTeacher2( )
{
    std::string courseId;
    std::cout << "请输入要打分的课程ID：";
    std::cin >> courseId;

    // 从teacher.json中遍历并输出课程名字
    teacher teacherObj;
    teacherObj.loadFromFile("teacher.json");
    course courseObj;
    courseObj.loadFromFile("course.json");
    std::string courseName;
    for( const auto& course : teacherObj.teacherCourses )
    {
        if( course.id == courseId )
        {
            courseName = course.name;
            break;
        }
    }
    if( courseName.empty( ) )
    {
        std::cout << "未找到该课程ID的课程" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        clearScreen( );
        teacherMenu( );
        return;
    }
    std::cout << "要录入分数的课程名称：" << courseName << std::endl;

    // 在course.json中搜索该课程id的数据，并展示学生ID列表
    std::vector<studentOfCourse> students;
    courseObj.loadFromFileFixedId("course.json", courseId);
    students = courseObj.courseStudents;
    if( students.empty( ) )
    {
        std::cout << "该课程暂无学生选修" << std::endl;
        return;
    }
    std::cout << "学生ID列表：" << std::endl;
    for( const auto& student : students )
    {
        std::cout << "学生ID：" << student.id << "  学生目前分数：" << student.grade << std::endl;
    }

    // 输入打分
    bool continueGivingScore = true;
    while( continueGivingScore )
    {
        std::string studentId;
        double score;
        std::cout << "请输入学生ID和分数（以空格分隔）：" << std::endl;
        std::cin >> studentId >> score;

        // 更新分数并保存到course.json和student.json中

        for( auto& student : courseObj.courseStudents )
        {
            if( student.id == studentId )
            {
                student.grade = score;
                break;
            }
        }

        courseObj.saveToFile("course.json");

        // 将学生打分信息存入student.json文件中
        student studentObj;
        studentObj.loadFromFileFixedId("student.json", studentId);

        // 将所有课程的成绩合并到 studentObj 中
        std::vector<courseStoredStudent>& studentCourses = studentObj.studentCourses;

        std::vector<courseStoredStudent> allCourses;
        for( const auto& course : studentCourses )
        {
            courseStoredStudent courseData;
            courseData.id = course.id;
            courseData.name = course.name;
            courseData.courseCredit = course.courseCredit;
            courseData.grade = course.grade;
            courseData.GPA = course.GPA;
            allCourses.push_back(courseData);
        }

        // 在 allCourses 容器中寻找与 courseId 相同的课程进行打分
        for( auto& course : allCourses )
        {
            if( course.id == courseId )
            {
                course.grade = score;
                course.GPA = convertToGPA(score);
                break;
            }
        }

        // 更新 studentCourses 容器中对应课程的分数和 GPA
        for( auto& course : studentCourses )
        {
            if( course.id == courseId )
            {
                course.grade = score;
                course.GPA = convertToGPA(score);
                break;
            }
        }

        studentObj.saveToFile("student.json");

        std::cout << "打分成功！" << std::endl;

        bool validInput = false;
        while( !validInput )
        {
            std::cout << "是否继续打分 [Y/N]: ";
            char gradingYesOrNo;
            std::cin >> gradingYesOrNo;

            if( gradingYesOrNo == 'Y' || gradingYesOrNo == 'y' )
            {
                continueGivingScore = true;
                validInput = true;
            }
            else if( gradingYesOrNo == 'N' || gradingYesOrNo == 'n' )
            {
                continueGivingScore = false;
                validInput = true;
            }
            else
            {
                std::cout << "无效的输入，请重新输入" << std::endl;
            }
        }
    }
    clearScreen( );
    teacherMenu( );
}

// 教师功能3：课程查分
void menuFunctionTeacher3( )
{
    std::cout << "请输入需要查询的课程id:\n";
    std::string courseId;
    std::cin >> courseId;
    // 加载课程数据
    course courseObj;
    courseObj.loadFromFileFixedId("course.json", courseId);

    // 检查课程数据是否存在
    if( courseObj.courseStudents.empty( ) )
    {
        std::cout << "该课程暂无学生选修" << std::endl;
        return;
    }

    // 统计课程成绩
    double totalScore = 0.0;
    double maxScore = std::numeric_limits<double>::min( );
    double minScore = std::numeric_limits<double>::max( );
    int studentCount = 0;

    for( const auto& student : courseObj.courseStudents )
    {
        double score = student.grade;
        totalScore += score;
        maxScore = std::max(maxScore, score);
        minScore = std::min(minScore, score);
        studentCount++;
    }

    // 计算平均分
    double averageScore = totalScore / studentCount;

    // 输出统计结果
    std::cout << "课程ID: " << courseId << std::endl;
    std::cout << "课程名称：" << courseObj.name << std::endl;
    std::cout << "课程教师：" << courseObj.teacherName << std::endl;
    std::cout << std::endl;
    std::cout << "学生人数: " << studentCount << std::endl;
    std::cout << "平均分: " << averageScore << std::endl;
    std::cout << "最高分: " << maxScore << std::endl;
    std::cout << "最低分: " << minScore << std::endl;

    std::cout << "(10秒后返回教师主界面)\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));
    clearScreen( );
    teacherMenu( );
}

// 学生功能1：选择课程
void menuFunctionStudent1( )
{
    student stu;
    stu.chooseCourse( );
    clearScreen( );
    studentMenu( );
}

// 学生功能2：退出课程
void menuFunctionStudent2( )
{
    student stu;
    stu.quitCourse( );
    clearScreen( );
    studentMenu( );
}

// 学生功能3：课程查分
void menuFunctionStudent3( )
{
    student stu;
    stu.checkGrade( );
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

// 学生功能4：查询均绩
void menuFunctionStudent4( )
{
    student stu;
    stu.calculateGPA( );
    std::cout << "(3秒后自动返回学生主菜单)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

int main( )
{
    mainMenu( );

    return 0;
}