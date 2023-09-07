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

// ȫ�ֱ���
std::string userIdNow;
std::string userTypeNow;
std::string userNameNow;

// ��Ҫ��ǰ�����ĺ���
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

// �ٷ����뼨��ת������
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

// object��
class object
{
public:
    virtual void set( ) = 0;
    virtual void show( ) = 0;
    std::string id;
    std::string name;
};

// people��
class people : public object
{
public:
    void set( ) override;
    void show( ) override;

private:
};

// people���set����override
void people::set( )
{
    std::cout << "������ѧ���ţ�\n";
    std::cin >> id;
    std::cout << "������������\n";
    std::cin >> name;
}

// people���show����override
void people::show( )
{
    std::cout << "ѧ���ţ�" << id << std::endl;
    std::cout << "������" << name << std::endl;
}

// �򻯰�course�࣬������student���´�������
class courseStoredStudent
{
public:
    std::string id;
    std::string name;
    double courseCredit;
    double grade = 0;
    double GPA = 0;
};

// �򻯰�ѧ���࣬����course����vector������������
class studentOfCourse
{
public:
    std::string id;
    std::string name;
    double grade = 0;
};

// course��
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

// ��course����ת��Ϊjson����
json course::toJson( ) const
{
    json j;
    j["id"] = id;
    j["name"] = name;
    j["teacher"] = teacherName;
    j["credit"] = courseCredit;

    // ��ѡ��˿γ�ѧ����Ϣת��Ϊ JSON ����
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

// ��json����ת��Ϊcourse����
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

// ��course�����Ϊjson�ļ�
void course::saveToFile(const std::string& filename) const
{
    json jsonCourseArray = json::array( ); // ���մ������һ��json����
    json jsonCourseData;                  // ��ʱ����ȷ���ļ��ж����jsonCourseData��json������json����
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
        //std::cout << "�����ѱ�����" << filename << std::endl;
    }
    else
    {
        std::cout << "�޷����������� " << filename << std::endl;
    }
}

// ��json�ļ��м��ط��ϵ�ǰid��course����
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
            if( jsonCourseArray["id"].get<std::string>( ) == id ) // ��bug�����jsonCourseArray�ǿ�json�����������Խ��
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
        //std::cout << "�����Ѵ�" << filename << " ���ء�" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "�������ļ� " << filename << " �ɹ���" << std::endl;
        }
        else
        {
            std::cout << "�޷��������ļ� " << filename << std::endl;
        }
    }

    //std::cout << "δ�ҵ����ϵ�ǰid��course�������ݡ�" << std::endl;
}

// ��json�ļ��м��ط��ϵ�ǰid��course������Ҫָ��Ҫ���صĿγ�id
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
            if( jsonCourseArray["id"].get<std::string>( ) == fixedId ) // ��bug�����jsonCourseArray�ǿ�json�����������Խ��
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
        //std::cout << "�����Ѵ�" << filename << " ���ء�" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "�������ļ� " << filename << " �ɹ���" << std::endl;
        }
        else
        {
            std::cout << "�޷��������ļ� " << filename << std::endl;
        }
    }

    //std::cout << "δ�ҵ����ϵ�ǰid��course�������ݡ�" << std::endl;
}

// �򻯰�course�࣬������teacher���´�������
class courseStoredTeacher
{
public:
    std::string id;
    std::string name;
    double courseCredit;
    double averageGrade = 0;
    double averageGPA = 0;
};

// course��set����override
void course::set( )
{
    std::cout << "������ѧ���ɼ���\n";
}

// course��show����override
void course::show( )
{
    std::cout << "չʾѧ���ɼ���\n";
}

// student��
class student : public people
{
public:
    student( );
    void chooseCourse( );
    void quitCourse( );
    void checkGrade( );
    void calculateGPA( );
    json toJson( ) const;                                // ��ѧ������ת��Ϊ JSON ����
    void fromJson(const json& j);                       // �� JSON ����ָ�ѧ������
    void saveToFile(const std::string& filename) const; // ��ѧ�����󱣴浽 JSON �ļ�
    void loadFromFile(const std::string& filename);     // �� JSON �ļ�����ѧ������
    void loadFromFileFixedId(const std::string& filename, std::string fixId);
    friend void menuFunctionTeacher2( );
private:
    std::vector<courseStoredStudent> studentCourses;
};

// student�๹�캯������ȡȫ�ֱ���id��name
student::student( )
{
    id = userIdNow;
    name = userNameNow;
    // std::cout << "��ǰ�˺�Ϊ��" << userIdNow << ",��ӭ " << userNameNow << "!" << std::endl;
}

// studentѡ�κ���
void student::chooseCourse( )
{
    // ���ر�ѡ�γ�
    courseStoredStudent chooseACourse;
    std::cout << "������ѡ��γ̿���ţ�\n";
    std::cin >> chooseACourse.id;
    course chosedCourse;
    chosedCourse.loadFromFileFixedId("course.json", chooseACourse.id);

    loadFromFile("student.json");

    // ���ѡûѡ���ÿγ�
    for( const auto& course : studentCourses )
    {
        if( course.id == chooseACourse.id )
        {
            std::cout << "����ѡ���ÿγ̣�\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
    }

    // ѡ����Ϣд��student -> <vector>courseStoredStudent
    chooseACourse.name = chosedCourse.name;
    chooseACourse.courseCredit = chosedCourse.courseCredit;
    studentCourses.push_back(chooseACourse);
    saveToFile("student.json");

    // ѡ����Ϣд��course -> <vector>courseStudents
    studentOfCourse aNewStudent;
    aNewStudent.id = id;
    aNewStudent.name = name;
    chosedCourse.courseStudents.push_back(aNewStudent);
    chosedCourse.saveToFile("course.json");
    std::cout << "��ѡ��γ�" << chosedCourse.name << std::endl;
    std::cout << "(3����Զ�����)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

// student�˿κ���
void student::quitCourse( )
{
    // �����˿ογ�
    courseStoredStudent quitACourse;
    std::cout << "�������˿ογ̿γ̺ţ�\n";
    std::cin >> quitACourse.id;
    course quitedCourse;
    quitedCourse.loadFromFileFixedId("course.json", quitACourse.id);

    // �˿���Ϣд��student -> <vector>courseStoredStudent
    loadFromFile("student.json");

    // ɾ��studentCourses������idΪquitedCourse.id�Ķ���
    studentCourses.erase(
        std::remove_if(studentCourses.begin( ), studentCourses.end( ),
            [quitACourse] (const courseStoredStudent& course)
            {
                return course.id == quitACourse.id;
            }),
        studentCourses.end( ));

    saveToFile("student.json");

    // �˿���Ϣд��course -> <vector>courseStudents
    studentOfCourse aGoStudent;
    aGoStudent.id = id;

    // ɾ��courseStudents�����д�id��ѧ��
    quitedCourse.courseStudents.erase(
        std::remove_if(quitedCourse.courseStudents.begin( ), quitedCourse.courseStudents.end( ),
            [this] (const studentOfCourse& student)
            {
                return student.id == id;
            }),
        quitedCourse.courseStudents.end( ));

    quitedCourse.saveToFile("course.json");
    std::cout << "���˳��γ�" << quitedCourse.name << std::endl;
    std::cout << "(3����Զ�����)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

// ��student����תΪjson����
json student::toJson( ) const
{
    json j;
    j["id"] = id;
    j["name"] = name;

    // ��ѧ��ѡ����Ϣת��Ϊ JSON ����
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

// ��json����תΪstudent����
void student::fromJson(const json& j)
{
    id = j["id"].get<std::string>( );
    name = j["name"].get<std::string>( );

    // �� JSON ����ָ�ѧ��ѡ����Ϣ
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

// ��student�����Ϊjson�ļ�
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
        //std::cout << "�����ѱ�����" << filename << std::endl;
    }
    else
    {
        std::cout << "�޷����������� " << filename << std::endl;
    }
}

// ��json�ļ��м��ط��ϵ�ǰid��student����
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
            if( id == jsonStudentArray["id"].get<std::string>( ) ) // ��bug�����jsonStudentArray�ǿ�json�����������Խ��
            {
                fromJson(jsonStudentArray);
            }
            else if( id != jsonStudentArray["id"].get<std::string>( ) )
            {
                return;
            }
        }
        //std::cout << "�����Ѵ�" << filename << " ���ء�" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "�������ļ� " << filename << " �ɹ���" << std::endl;
        }
        else
        {
            std::cout << "�޷��������ļ� " << filename << std::endl;
        }
    }
}

// ��json�ļ��м��ط��ϵ�ǰid��student�������ṩstudent��id
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
            if( fixId == jsonStudentArray["id"].get<std::string>( ) ) // ��bug�����jsonStudentArray�ǿ�json�����������Խ��
            {
                fromJson(jsonStudentArray);
            }
            else if( fixId != jsonStudentArray["id"].get<std::string>( ) )
            {
                return;
            }
        }
        //std::cout << "�����Ѵ�" << filename << " ���ء�" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "�������ļ� " << filename << " �ɹ���" << std::endl;
        }
        else
        {
            std::cout << "�޷��������ļ� " << filename << std::endl;
        }
    }
}

// ѧ����ֵĺ���
void student::checkGrade( )
{
    std::string courseId;
    std::cout << "������Ҫ��ֵĿγ�ID��";
    std::cin >> courseId;

    loadFromFile("student.json");

    bool found = false;
    for( const auto& course : studentCourses )
    {
        if( course.id == courseId )
        {
            std::cout << "�γ�ID: " << course.id << std::endl;
            std::cout << "�γ�����: " << course.name << std::endl;
            std::cout << "�γ̳ɼ�: " << course.grade << std::endl;
            std::cout << "�γ̼���: " << std::fixed << std::setprecision(1) << course.GPA << std::endl;
            std::cout << "(3����Զ�����ѧ�����˵�)\n";
            found = true;
            break;
        }
    }

    if( !found )
    {
        std::cout << "δ�ҵ��ÿγ�ID�ĳɼ���Ϣ" << std::endl;
    }
}

// ѧ���������
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
    std::cout << "��ã�" << name << "!\n";
    std::cout << "���ƽ��ѧ�ּ�Ϊ��\n";
    std::cout << std::fixed << std::setprecision(3) << averageGPA << std::endl;
}

// teacher��
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

// teacher�๹�캯������ȡȫ�ֱ���id��name
teacher::teacher( )
{
    id = userIdNow;
    name = userNameNow;
    // std::cout << "��ǰ�˺�Ϊ��" << userIdNow << ",��ӭ " << userNameNow << "!" << std::endl;
}

// ��ʦ����γ�
void teacher::startCourse( )
{
    // ����teacher -> <vector>course��
    courseStoredTeacher startACourse;
    std::cout << "�����뿪��γ̿���ţ�\n";
    std::cin >> startACourse.id;
    std::cout << "�����뿪��γ����ƣ�\n";
    std::cin >> startACourse.name;
    std::cout << "������γ�ѧ�֣�\n";
    std::cin >> startACourse.courseCredit;
    teacherCourses.push_back(startACourse);

    // ����course.json
    course newCourse;
    newCourse.id = startACourse.id;
    newCourse.name = startACourse.name;
    newCourse.teacherName = name;
    newCourse.courseCredit = startACourse.courseCredit;
    newCourse.saveToFile("course.json");
}

// ��teacher����תΪjson����
json teacher::toJson( ) const
{
    json j;
    j["id"] = id;
    j["name"] = name;

    // ����ʦ�Ŀγ���Ϣת��Ϊ JSON ����
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

// ��json����תΪteacher����
void teacher::fromJson(const json& j)
{
    id = j["id"].get<std::string>( );
    name = j["name"].get<std::string>( );

    // �� JSON ����ָ���ʦ�Ŀγ���Ϣ
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

// ��teacher�����Ϊjson�ļ�
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
        //std::cout << "�����ѱ�����" << filename << std::endl;
    }
    else
    {
        std::cout << "�޷���������" << filename << std::endl;
    }
}

// ��json�ļ��м��ط��ϵ�ǰid��teacher����
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
        else if( jsonTeacherArray.is_object( ) ) // ��bug�����jsonTeacherArray�ǿ�json�����������Խ��
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
        //std::cout << "�����Ѵ� " << filename << " ���ء�" << std::endl;
    }
    else
    {
        std::ofstream newfile(filename);
        if( newfile.is_open( ) )
        {
            newfile << "{}";
            newfile.close( );
            std::cout << "�������ļ� " << filename << " �ɹ���" << std::endl;
        }
        else
        {
            std::cout << "�޷��������ļ� " << filename << std::endl;
        }
    }
}

// ����û����Ƿ�ռ��
bool isUsernameTaken(const std::string& username)
{
    std::ifstream file("users.txt");
    std::string storedUsername, storedPassword;

    while( file >> storedUsername >> storedPassword )
    {
        if( storedUsername == username )
        {
            file.close( );
            return true; // �û����Ѵ���
        }
    }

    file.close( );
    return false; // �û���δ��ռ��
}

// �û�ע��ʹ�ú���
void registerUser( )
{
    std::string userId, password, passwordAgain, role, roletag, userName;

    // ���ѧ�����Ƿ�ע��
    std::ifstream fileCheck("users.txt");
    if( fileCheck.is_open( ) )
    {
        std::string storedUserId, storedPassword;
        do
        {
            std::cout << "������ѧ���ţ�";
            std::cin >> userId;

            if( isUsernameTaken(userId) )
            {
                std::cout << "ѧ�����ѱ�ռ�ã����������룺" << std::endl;
            }
        } while( isUsernameTaken(userId) );
    }

    std::cout << "���������룺";
    std::cin >> password;
    std::cout << "���ٴ��������룺";
    std::cin >> passwordAgain;

    // �������벻ͬʱ��Ҫ����������
    while( password != passwordAgain )
    {
        std::cout << "���������ȷ������������ͬ��" << std::endl;
        std::cout << "���������룺";
        std::cin >> password;
        std::cout << "���ٴ��������룺";
        std::cin >> passwordAgain;
    }
    std::cout << "�������û�����\n";
    std::cin >> userName;
    std::cout << "�������ǽ�ʦ����ѧ����\n��ʦѡt��ѧ��ѡs��\n";
    std::cin >> roletag;
    if( roletag == "t" )
        role = "teacher";
    else if( roletag == "s" )
        role = "student";
    // ���ļ��Ա����û���Ϣ
    std::ofstream file("users.txt", std::ios::app);
    if( file.is_open( ) )
    {
        file << userId << " " << password << " "
            << " " << userName << " " << role << std::endl;
        file.close( );
        std::cout << "ע��ɹ���" << std::endl << "(3��󷵻�)\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        clearScreen( );
        mainMenu( );
    }
    else
    {
        std::cout << "Error opening file." << std::endl;
    }
}

// ��¼����û��������뺯��
void loginUser( )
{
    std::string userId, password;
    std::cout << "������ѧ���ţ�";
    std::cin >> userId;

    // ���ļ��Լ���û���Ϣ
    std::ifstream file("users.txt");
    if( file.is_open( ) )
    {
        std::string storedUserId, storedPassword, userName, userType;
        file.seekg(0, std::ios::beg);
        while( file >> storedUserId >> storedPassword >> userName >> userType )
        {
            if( storedUserId == userId )
            {
                std::cout << "���������룺";
                std::cin >> password;
                while( password != storedPassword )
                {
                    std::cout << "����������������룺";
                    std::cin >> password;
                }
                userIdNow = userId;
                userTypeNow = userType;
                userNameNow = userName;
                std::cout << "��½�ɹ���\n";
                file.close( );
                return;
            }
        }
    }
    std::cout << "�û������ڣ�" << std::endl;
    file.close( );
}

// �������ھ�������ı�
void printCentered(const std::string& text, int width)
{
    int padding = ( width - text.length( ) ) / 2;
    std::cout << std::setw(padding + text.length( )) << std::setfill(' ') << text << std::endl;
}

// ������������˵���
void printMenuItem(const std::string& text, int width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << text;
}

// ��տ���̨����ĺ���
void clearScreen( )
{
#ifdef _WIN32 // Windows ϵͳ
    system("cls");
#elif defined(__linux__) || defined(__APPLE__) // Linux �� macOS ϵͳ
    system("clear");
#else
    // ��������ϵͳ���޷��Զ�ʶ��
    // ���Ը���һ��������ʾ��ʹ��Ĭ�ϵ������Ļ����
    std::cout << "�޷��Զ�ʶ�����ϵͳ�����ֶ������Ļ��" << std::endl;
#endif
}

// ��¼������
void mainMenu( )
{
    /*��¼������*/
    int loginRegisterChoice;
    /*���Ʋ˵�������*/

    // ����˵�����Ͳ˵���
    std::string title = "��ӭ����ѧ���ɼ�����ϵͳ��";
    std::string tip = "����������ѡ����ţ�";
    std::string menuItem1 = "1. ʦ����¼";
    std::string menuItem2 = "2. ʦ��ע��";
    std::string menuItem3 = "3. �˳�ϵͳ";

    // ��������Ⱥͱ߿��ַ�
    int width = 30;
    char borderChar = '*';

    // ������涥���߿�
    std::cout << std::string(width, borderChar) << std::endl;

    // �������
    printCentered(title, width);

    // ����˵���
    std::cout << std::string(width, borderChar) << std::endl;
    printCentered(menuItem1, width);
    printCentered(menuItem2, width);
    printCentered(menuItem3, width);

    // �������ײ��߿�
    std::cout << std::string(width, borderChar) << std::endl;

    /****************/

    std::cin >> loginRegisterChoice;
    while( loginRegisterChoice != 1 && loginRegisterChoice != 2 && loginRegisterChoice != 3 )
    {
        std::cout << "��Чѡ����������룺" << std::endl;
        std::cin >> loginRegisterChoice;
    }
    if( loginRegisterChoice == 1 )
    {
        clearScreen( );
        std::cout << "������ >> ��¼\n";
        loginUser( );
        if( userTypeNow == "teacher" )
        {
            clearScreen( );
            std::cout << "������ >> ��¼ >> ��ʦ����" << std::endl;
            teacherMenu( );
        }
        else if( userTypeNow == "student" )
        {
            clearScreen( );
            std::cout << "������ >> ��¼ >> ѧ������" << std::endl;
            studentMenu( );
        }
    }
    else if( loginRegisterChoice == 2 )
    {
        clearScreen( );
        std::cout << "������ >> ע��\n";
        registerUser( );
    }
    else if( loginRegisterChoice == 3 )
    {
        clearScreen( );
        std::cout << "�������˳�\n";
        std::cout << "ллʹ�ã��ټ���\n";
        std::exit(1);
    }
    /************/
}

// ѧ������˵�����
void studentMenu( )
{
    /*���Ʋ˵�������*/

    // ����˵�����Ͳ˵���
    std::string title = "ѧ���ɼ�����ϵͳ����ѧ����";
    std::string menuItem1 = "1. ѡ��γ�";
    std::string menuItem2 = "2. �˳��γ�";
    std::string menuItem3 = "3. �γ̲��";
    std::string menuItem4 = "4. ��ѯ����";
    std::string menuItem5 = "5. �˳�ϵͳ";

    // ��������Ⱥͱ߿��ַ�
    int width = 30;
    char borderChar = '*';

    // ������涥���߿�
    std::cout << std::endl;
    std::cout << std::string(width, borderChar) << std::endl;

    // �������
    printCentered(title, width);

    // ����˵���
    std::cout << std::string(width, borderChar) << std::endl;
    printCentered(menuItem1, width);
    printCentered(menuItem2, width);
    printCentered(menuItem3, width);
    printCentered(menuItem4, width);
    printCentered(menuItem5, width);
    std::cout << std::endl;

    // �������ײ��߿�
    std::cout << std::string(width, borderChar) << std::endl;

    /****************/

    /*����ѡ��*/
    std::cout << std::endl;
    std::cout << "��������Ҫ���еĲ�����" << std::endl;
    std::cout << "(�����Ӧ��������ż���)" << std::endl;
    int menuItemChoice;
    std::cin >> menuItemChoice;
    switch( menuItemChoice )
    {
    case 1:
        clearScreen( );
        std::cout << "�ѽ��빦��1��ѡ��γ�" << std::endl;
        menuFunctionStudent1( );
        break;
    case 2:
        clearScreen( );
        std::cout << "�ѽ��빦��2���˳��γ�" << std::endl;
        menuFunctionStudent2( );
        break;
    case 3:
        clearScreen( );
        std::cout << "�ѽ��빦��3���γ̲��" << std::endl;
        menuFunctionStudent3( );
        break;
    case 4:
        clearScreen( );
        std::cout << "�ѽ��빦��4����ѯ����" << std::endl;
        menuFunctionStudent4( );
        break;
    case 5:
        clearScreen( );
        std::cout << "�������˳�\n";
        std::cout << "ллʹ�ã��ټ���\n";
        std::exit(1);
    default:
        std::cout << "����ѡ����Ч��������ѡ��" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        clearScreen( );
        studentMenu( );
        break;
    }
    /**********/
}

// ��ʦ����˵�����
void teacherMenu( )
{
    /*���Ʋ˵�������*/

    // ����˵�����Ͳ˵���
    std::string title = "ѧ���ɼ�����ϵͳ������ʦ��";
    std::string menuItem1 = "1. ����γ�";
    std::string menuItem2 = "2. �γ̴��";
    std::string menuItem3 = "3. �γ̲��";
    std::string menuItem4 = "4. �˳�ϵͳ";

    // ��������Ⱥͱ߿��ַ�
    int width = 30;
    char borderChar = '*';

    // ������涥���߿�
    std::cout << std::endl;
    std::cout << std::string(width, borderChar) << std::endl;

    // �������
    printCentered(title, width);

    // ����˵���
    std::cout << std::string(width, borderChar) << std::endl;
    printCentered(menuItem1, width);
    printCentered(menuItem2, width);
    printCentered(menuItem3, width);
    printCentered(menuItem4, width);
    std::cout << std::endl;

    // �������ײ��߿�
    std::cout << std::string(width, borderChar) << std::endl;

    /****************/

    /*����ѡ��*/
    std::cout << std::endl;
    std::cout << "��������Ҫ���еĲ�����" << std::endl;
    std::cout << "(�����Ӧ��������ż���)" << std::endl;
    int menuItemChoice;
    std::cin >> menuItemChoice;
    switch( menuItemChoice )
    {
    case 1:
        clearScreen( );
        std::cout << "�ѽ��빦��1������γ�" << std::endl;
        menuFunctionTeacher1( );
        break;
    case 2:
        clearScreen( );
        std::cout << "�ѽ���γ�2���γ̴��" << std::endl;
        menuFunctionTeacher2( );
        break;
    case 3:
        clearScreen( );
        std::cout << "�ѽ���γ�3���γ̲��" << std::endl;
        menuFunctionTeacher3( );
        break;
    case 4:
        clearScreen( );
        std::cout << "�������˳�\n";
        std::cout << "ллʹ�ã��ټ���\n";
        std::exit(1);
    default:
        std::cout << "����ѡ����Ч��������ѡ��" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        clearScreen( );
        teacherMenu( );
        break;
    }
    /**********/
}

// ��ʦ����1������γ�
void menuFunctionTeacher1( )
{
    teacher tea;
    tea.loadFromFile("teacher.json");
    tea.startCourse( );
    tea.saveToFile("teacher.json");
}

// ��ʦ����2���γ̴��
void menuFunctionTeacher2( )
{
    std::string courseId;
    std::cout << "������Ҫ��ֵĿγ�ID��";
    std::cin >> courseId;

    // ��teacher.json�б���������γ�����
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
        std::cout << "δ�ҵ��ÿγ�ID�Ŀγ�" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        clearScreen( );
        teacherMenu( );
        return;
    }
    std::cout << "Ҫ¼������Ŀγ����ƣ�" << courseName << std::endl;

    // ��course.json�������ÿγ�id�����ݣ���չʾѧ��ID�б�
    std::vector<studentOfCourse> students;
    courseObj.loadFromFileFixedId("course.json", courseId);
    students = courseObj.courseStudents;
    if( students.empty( ) )
    {
        std::cout << "�ÿγ�����ѧ��ѡ��" << std::endl;
        return;
    }
    std::cout << "ѧ��ID�б�" << std::endl;
    for( const auto& student : students )
    {
        std::cout << "ѧ��ID��" << student.id << "  ѧ��Ŀǰ������" << student.grade << std::endl;
    }

    // ������
    bool continueGivingScore = true;
    while( continueGivingScore )
    {
        std::string studentId;
        double score;
        std::cout << "������ѧ��ID�ͷ������Կո�ָ�����" << std::endl;
        std::cin >> studentId >> score;

        // ���·��������浽course.json��student.json��

        for( auto& student : courseObj.courseStudents )
        {
            if( student.id == studentId )
            {
                student.grade = score;
                break;
            }
        }

        courseObj.saveToFile("course.json");

        // ��ѧ�������Ϣ����student.json�ļ���
        student studentObj;
        studentObj.loadFromFileFixedId("student.json", studentId);

        // �����пγ̵ĳɼ��ϲ��� studentObj ��
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

        // �� allCourses ������Ѱ���� courseId ��ͬ�Ŀγ̽��д��
        for( auto& course : allCourses )
        {
            if( course.id == courseId )
            {
                course.grade = score;
                course.GPA = convertToGPA(score);
                break;
            }
        }

        // ���� studentCourses �����ж�Ӧ�γ̵ķ����� GPA
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

        std::cout << "��ֳɹ���" << std::endl;

        bool validInput = false;
        while( !validInput )
        {
            std::cout << "�Ƿ������� [Y/N]: ";
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
                std::cout << "��Ч�����룬����������" << std::endl;
            }
        }
    }
    clearScreen( );
    teacherMenu( );
}

// ��ʦ����3���γ̲��
void menuFunctionTeacher3( )
{
    std::cout << "��������Ҫ��ѯ�Ŀγ�id:\n";
    std::string courseId;
    std::cin >> courseId;
    // ���ؿγ�����
    course courseObj;
    courseObj.loadFromFileFixedId("course.json", courseId);

    // ���γ������Ƿ����
    if( courseObj.courseStudents.empty( ) )
    {
        std::cout << "�ÿγ�����ѧ��ѡ��" << std::endl;
        return;
    }

    // ͳ�ƿγ̳ɼ�
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

    // ����ƽ����
    double averageScore = totalScore / studentCount;

    // ���ͳ�ƽ��
    std::cout << "�γ�ID: " << courseId << std::endl;
    std::cout << "�γ����ƣ�" << courseObj.name << std::endl;
    std::cout << "�γ̽�ʦ��" << courseObj.teacherName << std::endl;
    std::cout << std::endl;
    std::cout << "ѧ������: " << studentCount << std::endl;
    std::cout << "ƽ����: " << averageScore << std::endl;
    std::cout << "��߷�: " << maxScore << std::endl;
    std::cout << "��ͷ�: " << minScore << std::endl;

    std::cout << "(10��󷵻ؽ�ʦ������)\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));
    clearScreen( );
    teacherMenu( );
}

// ѧ������1��ѡ��γ�
void menuFunctionStudent1( )
{
    student stu;
    stu.chooseCourse( );
    clearScreen( );
    studentMenu( );
}

// ѧ������2���˳��γ�
void menuFunctionStudent2( )
{
    student stu;
    stu.quitCourse( );
    clearScreen( );
    studentMenu( );
}

// ѧ������3���γ̲��
void menuFunctionStudent3( )
{
    student stu;
    stu.checkGrade( );
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

// ѧ������4����ѯ����
void menuFunctionStudent4( )
{
    student stu;
    stu.calculateGPA( );
    std::cout << "(3����Զ�����ѧ�����˵�)\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    clearScreen( );
    studentMenu( );
}

int main( )
{
    mainMenu( );

    return 0;
}