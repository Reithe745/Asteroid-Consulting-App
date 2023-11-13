#define CURL_STATICLIB
//teste na nova branch
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "nlohmann/json.hpp"
#include "curl/curl.h"

using json = nlohmann::json;
using namespace std;

class Information {

private:
    string date, standartDate = "2000-01-01";
    int year, month, day;
    vector<double> sizes;
    vector<double> dist;
    string link;
    json information;

public:

    bool dateInvalid = false;

   string returnDateDefined() {
       return date;
    }

    void setJson(string info) {
        information = json::parse(info);
    }

    void getBiggestDiameter() {

        double val1, val2, res;

        for (const auto& object : information["near_earth_objects"][date]) {

            val1 = object["estimated_diameter"]["kilometers"]["estimated_diameter_min"];
            val2 = object["estimated_diameter"]["kilometers"]["estimated_diameter_max"];
            res = (val1 * val2)/2;

            sizes.push_back(res);
        }

        cout << " > Biggest diameter -> " << *max_element(sizes.begin(), sizes.end()) << " km" << endl;
    }

    void getBiggestApproach() {

        double num;
        string txt;

        for (const auto& object : information["near_earth_objects"][date]) {
            
            txt = object["close_approach_data"][0]["miss_distance"]["kilometers"];

            num = stod(txt);

            dist.push_back(num/1000);
        }

        cout << " > Closest element to earth -> " << *min_element(dist.begin(), dist.end()) << " mil km" << endl;
    }

    void setupDateTimeForQuery() {

        cout << "- YEAR" << endl;
        cin >> year;
        cout << "- MONTH" << endl;
        cin >> month;
        cout << "- DAY" << endl;
        cin >> day;

        if (day > 31 || day < 1 || year > 2023 || year < 1940 || month > 12 || month < 1) {
            date = standartDate;
            dateInvalid = true;
        }
        else {
            if (day < 10) {

                date = to_string(year) + "-" + to_string(month) + "-0" + to_string(day);

                if (month < 10) {

                    date = to_string(year) + "-0" + to_string(month) + "-0" + to_string(day);
                }
            }
            else {
                date = to_string(year) + "-" + to_string(month) + "-" + to_string(day);

                if (month < 10) {

                    date = to_string(year) + "-0" + to_string(month) + "-" + to_string(day);
                }
            }
        }       
    }
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void CurlSetup(string date, Information& a) {

    CURL* curl;
    CURLcode res;
    string response_string;

    curl = curl_easy_init();

    if (curl) {
        string url_beggin = "https://api.nasa.gov/neo/rest/v1/feed?start_date=";
        string url_mid = "&end_date=";
        string url_end = "&api_key=dDhYLcgGcjUEgaSsMSCxtjwwy0hA1fq1S75a6oWR";

        string url = url_beggin + date + url_mid + date + url_end;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "Erro na requisição: " << curl_easy_strerror(res) << endl;
        }

        curl_easy_cleanup(curl);
    }

    a.setJson(response_string);
}

void print_text(string text, int linesSkiped) {
    switch (linesSkiped){
    case 0:
        cout << text;
        break;
    case 1:
        cout << text << endl;
        break;
    case 2:
        cout << text << endl << endl;
        break;
    default:
        cout << text;
        break;
    }
}

int main() {

    Information InformationObject;

    print_text("Welcome to METEOR TRACKER, a meteor query tool", 1);
    print_text("Inform YEAR, MONTH and DAY of your desire", 1);
    print_text("You'll be presente with the one meteor with the biggest DIAMETER and CLOSES it got to earth", 2);

    print_text("Year format must be between 1940 and 2022", 2);

    InformationObject.setupDateTimeForQuery();

    system("CLS");

    CurlSetup(InformationObject.returnDateDefined(), InformationObject);

    if (InformationObject.dateInvalid) {
        cout << "DATE WAS INVALID, new date: " << InformationObject.returnDateDefined() << endl << endl;
    }
    else {
        cout << "Those are the informations queried on the date: " << InformationObject.returnDateDefined() << endl << endl;
    }
    
    InformationObject.getBiggestDiameter();
    InformationObject.getBiggestApproach();

    print_text("", 1);
    print_text("this query was made possible by the Nasa's API", 1);
    print_text("it's documentations can be consulted on the link: https://api.nasa.gov/", 1);

    system("pause");

    return 0;
}