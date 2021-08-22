// Test program to learn how to use TermOx library

#include <unistd.h>
#include <termox/termox.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/bordered.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/widgets/text_view.hpp>
#include <termox/widget/widgets/textbox.hpp>
#include <termox/widget/widgets/scrollbar.hpp>
#include <termox/widget/widgets/line_edit.hpp>
#include <termox/widget/widgets/log.hpp>

#include <string>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>
#include <fstream>

#include <unistd.h>
#include <time.h>

using namespace ox;


struct DisplayTime : public Bordered<Log> {
    public:
    Bordered<Log>& border = *this;
    Log &time_display = border.wrapped;
    
    DisplayTime() : Bordered<Log>{border::rounded()} 
    {
        using namespace pipe;
        time_display | bg(Color::Dark_gray);
        border | bg(Color::Black);

        using namespace std::chrono_literals;
        loop_.run_async([this](auto &queue) {
            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);
            std::string time_string{std::asctime(&tm)};
            queue.append(
                    Custom_event{[this, time_string] {
                        time_display.post_message(time_string);
            }});
            std::this_thread::sleep_for(1000ms);
        });
    }

    private:
        Event_loop loop_;
};

struct TextLog : public Bordered<Log> {
    public: 
    Bordered<Log>& border = *this;
    Log &log              = border.wrapped;

    void post_message(std::string const &val) {
        log.post_message(val);
    }

    TextLog() : Bordered<Log>{border::rounded()} {  } 
};

struct EntryField : public Bordered<Line_edit> {
    public:
    Bordered<Line_edit>& border = *this;
    Line_edit& field = border.wrapped;

    EntryField() : Bordered<Line_edit>{border::rounded()} { }
};

struct UserPane : public layout::Vertical<> {
    public:
    TextLog      &file_contents = this->make_child<TextLog>();
    Line_edit    &path_name     = this->make_child<Line_edit>("Type here");
    UserPane() {
        using namespace pipe;
        *this | fixed_width(200);
        path_name.submitted.connect([this](std::string const&val) {
            std::ifstream input(val);
            if (!input.is_open()) {
                std::string err{"Failed to open: "};
                file_contents.post_message(err+val); 
            } else {
                std::string line;
                while (std::getline(input, line)) {
                    file_contents.post_message(line);
                }
            }
            path_name.set_text("");  
        });
    }

};

struct Main_pane : public layout::Horizontal<> {
    public:
    UserPane      &user_pane    = this->make_child<UserPane>();
    DisplayTime   &time         = this->make_child<DisplayTime>();
    Main_pane() { }
};


int main() { return System{}.run<Main_pane>(); }
