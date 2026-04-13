#include <iostream>
#include <string>
#include <regex>
#include <utility>
#include <vector>
#include <algorithm>
#include "ext/picosha2.h"
#include <limits>

enum class OrderStatus { Procesare, InTransit, Livrat };

struct Order {
    int order_id;
    std::string curier_name;
    int total_price;
    float total_weight;
    OrderStatus status;
    [[nodiscard]] std::string status_to_string() const {
        switch (status) {
            case OrderStatus::Procesare:  return "In procesare";
            case OrderStatus::InTransit:  return "In transit";
            case OrderStatus::Livrat:     return "Livrat";
        }
        return "Necunoscut";
    }
    void print() const {
        std::cout << "Order ID    : " << order_id << "\n"
                  << "Curier      : " << curier_name << "\n"
                  << "Pret total  : " << total_price << " RON\n"
                  << "Greutate    : " << total_weight << " kg\n"
                  << "Status      : " << status_to_string() << "\n";
    }
};

// inspo https://stackoverflow.com/questions/5864540/infinite-loop-with-cin-when-typing-string-while-a-number-is-expected
static int safe_read() {
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Input invalid, incearca din nou: ";
    }
    return val;
}


class Product {
    std::string name;
    int stock_number;
    float weight;
    int ID;
    int price;

public:
    Product(std::string product_name, int product_stock_number, float product_weight, int product_id,
            int product_price) : name(std::move(product_name)), stock_number(product_stock_number),
                                 weight(product_weight), ID(product_id), price(product_price) {
    }

    Product(const Product &another_product) =default;

    Product &operator=(const Product &another_product) {
        if (this != &another_product) {
            name = another_product.name;
            stock_number = another_product.stock_number;
            weight = another_product.weight;
            ID = another_product.ID;
            price = another_product.price;
        }
        return *this;
    }

    ~Product() = default;

    [[nodiscard]] const std::string &get_name() const { return name; }
    [[nodiscard]] float get_weight() const { return weight; }
    [[nodiscard]] int get_ID() const { return ID; }
    [[nodiscard]] int get_price() const { return price; }

    friend std::ostream &operator<<(std::ostream &os, const Product &product) {
        os << "ID: " << product.ID
           << " Nume: " << product.name
           << " Pret: " << product.price
           << " Greutate: " << product.weight
           << " In stoc: " << product.stock_number;
        return os;
    }

    void set_name(const std::string &new_name) {
        name = new_name;
    }

    void set_stock_number(int new_stock_number) {
        stock_number = new_stock_number;
    }

    void set_weight(float new_weight) {
        weight = new_weight;
    }

    void set_price(int new_price) {
        price = new_price;
    }

    void scade_stock() {
        if (stock_number > 0)
            stock_number--;
    }
    void creste_stock() {
        stock_number++;
    }
    [[nodiscard]] static bool is_in_stock(const Product& p) { return p.stock_number > 0; }
    static void print_stock_status(const Product& p) {
        if (p.stock_number > 1)
            std::cout << "In stoc!\n";
        else if (p.stock_number == 1)
            std::cout << "Un singur produs in stoc!\n";
        else
            std::cout << "Stoc epuizat!\n";
    }
};

class Destinatar {
    std::string name;
    std::string address;
    std::string email;
    std::string phone_number;

    static bool validate_phone_number(const std::string &nr_tel) {
        if (nr_tel.length() != 10 || nr_tel[0] != '0' || nr_tel[1] != '7') {
            std::cout << "Nr de telefon invalid! \n";
            return false;
        }
        return true;
    }

    static bool validate_email(const std::string &email) {
        //inspo https://clehaxze.tw/gemlog/2023/01-28-validate-email-address-using-regex-in-cpp.gmi
        static const std::regex email_regex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
        std::smatch match;
        if (!std::regex_match(email, match, email_regex)) {
            std::cout << "Email invalid! \n Introdu un mail valid de tip: a@a.com! ";
            return false;
        }
        return true;
    }

public:
    Destinatar() =default ;

    Destinatar(std::string name_dest, std::string address_dest, std::string phone_number_dest,
               std::string email_dest) : name(std::move(name_dest)), address(std::move(address_dest)),
                                         email(std::move(email_dest)), phone_number(std::move(phone_number_dest)) {
    }

    friend std::istream &operator>>(std::istream &is, Destinatar &destinatar) {
        std::cout << "Name:";
        std::getline(is >> std::ws, destinatar.name);
        std::cout << "Address:";
        std::getline(is >> std::ws, destinatar.address);
        std::cout << "Phone number:";
        is >> destinatar.phone_number;
        while (!validate_phone_number(destinatar.phone_number)) {
            is >> destinatar.phone_number;
        }
        std::cout << "Email:";
        is >> destinatar.email;
        while (!validate_email(destinatar.email)) {
            is >> destinatar.email;
        }
        return is;
    }


    friend std::ostream &operator<<(std::ostream &os, const Destinatar &destinatar) {
        os << "Nume: " << destinatar.name << std::endl;
        os << "Adresa: " << destinatar.address << std::endl;
        os << "Telefon: " << destinatar.phone_number << std::endl;
        os << "Email: " << destinatar.email << std::endl;
        return os;
    }
};


class Colet {
    std::vector<std::string> content;
    float weight;
    int AWB;
    int price;
    Destinatar destinatar;

public:
    Colet(std::vector<std::string> box_content, float box_weight, int box_awb, int box_price,
          Destinatar destinatar_colet) : content(std::move(box_content)), weight(box_weight), AWB(box_awb),
                                         price(box_price), destinatar(std::move(destinatar_colet)) {
    }
    [[nodiscard]] int package_price() const {
        return price;
    }
    [[nodiscard]] float get_weight() const {
        return weight;
    }

    friend std::ostream &operator<<(std::ostream &os, const Colet &colet) {
        os << "AWB: " << colet.AWB << "\n";
        os << "Greutate: " << colet.weight << " kg\n";
        os << "Pret Total: " << colet.price << " RON\n";
        os << "Continut: ";
        for (const auto& item : colet.content) {
            os << item << "; ";
        }
        os << "\nDate Livrare\n";
        os << colet.destinatar;
        return os;
    }

    bool add_item_into_box(Product &chosen_product) {
        if (!Product::is_in_stock(chosen_product)) {
            std::cout << "Produsul nu este in stoc!\n";
            return false;
        }
        this->price += chosen_product.get_price();
        this->weight += chosen_product.get_weight();
        this->content.push_back(chosen_product.get_name());
        chosen_product.scade_stock();
        return true;
    }
    bool remove_item_from_box(Product &chosen_product) {
        auto it = std::find(content.begin(), content.end(), chosen_product.get_name());
        if (it != content.end()) {
            content.erase(it);
            this->price -= chosen_product.get_price();
            this->weight -= chosen_product.get_weight();
            chosen_product.creste_stock();
            return true;
        }
        return false;
    }
    void reset(int new_awb) {
        content.clear();
        weight = 0.0f;
        price  = 0;
        AWB    = new_awb;
    }
};

class Duba {
    const std::string numar_inmatriculare;
    int max_capacity;
    int tier;


public:
    Duba(std::string numar_inmatriculare_duba, int max_capacity_duba, int tier_duba) : numar_inmatriculare(std::move(numar_inmatriculare_duba)), max_capacity(max_capacity_duba), tier(tier_duba) {}

    [[nodiscard]] int get_max_capacity() const {
        return max_capacity;
    }
    [[nodiscard]] const std::string& get_numar_inmatriculare() const {
        return numar_inmatriculare;
    }
    [[nodiscard]] int duba_tier() const {
        return tier;
    }
    friend std::ostream &operator<<(std::ostream &os, const Duba &duba) {
        os << "Duba: " << duba.numar_inmatriculare
           << " Capacitate: " << duba.max_capacity
           << " Tier: " << duba.tier;
        return os;
    }
};

class Curier {
    const std::string phone_number;
    const std::string name;
    int tier;
    Duba duba;

public:
    Curier(std::string numar_telefon_curier, std::string nume_curier, int tier,Duba duba_curier) : phone_number(std::move(numar_telefon_curier)), name(std::move(nume_curier)),tier(tier), duba(std::move(duba_curier)) {}

    [[nodiscard]] bool receive_box(const Colet& package) const {
        double multiplier = 1.0 + (duba.duba_tier() * 0.10);
        double actual_capacity = this->duba.get_max_capacity() * multiplier;
        if (package.package_price() > 2000 && this->tier <= 2) {
            std::cout << "Eroare: Comenzile peste 2000 RON necesita un curier VIP (Tier 3+)!\n";
            return false;
        }
        if (package.get_weight()<=actual_capacity) {
            std::cout<<"Coletul va fi livrat de: "<<this->name << std::endl;
            std::cout<<"Contact: "<<this->phone_number<<" "<<this->duba.get_numar_inmatriculare()<<std::endl;
        }
        else {
            std::cout<<"Capacity excedeed! Choose another postman."<<std::endl;
            return false;
        }
        return true;
    }

    [[nodiscard]] const std::string& get_name() const {
        return name;
    }

    [[nodiscard]] int get_tier() const {
        return tier;
    }
    friend std::ostream &operator<<(std::ostream &os, const Curier &curier) {
        os << "Nume Curier: " << curier.name << "\n"
           << "Telefon: " << curier.phone_number << "\n"
           << "Tier: " << curier.tier << "\n"
           << "Masina: " << curier.duba;
        return os;
    }
};

class FancurierApp {

    std::vector<Product> products{
        Product("casti audio", 10, 1.0f, 1, 100),
        Product("masina de spalat", 1, 100.0f, 2, 2000),
        Product("cafea", 20, 0.5f, 3, 40),
        Product("deodorant", 30, 0.2f, 4, 15),
        Product("matura", 34, 0.2f, 5, 10)
    };
    std::vector <Duba> list_of_vans{
        Duba("B111AAA",1000,1),
        Duba("B222AAA",5000,2),
        Duba("B333AAA",10000,3),
        Duba("B444AAA",15000,4)
    };
    std::vector <Curier> list_of_postmans{
        Curier("0711111111","Nea caisa",1,list_of_vans[0]),
        Curier("0722222222","Nea gheorghe",2,list_of_vans[1]),
        Curier("0733333333","Adelin",3,list_of_vans[2]),
        Curier("0744444444","Domn Marcea",4,list_of_vans[3])
    };
    std::vector<Order> order_history;
    int next_order_id = 1000;
    void list_products() const {
        std::cout << "Lista de produse disponibile astazi: " << std::endl;
        for (const auto & product : products) {
            std::cout << "ID: " << product.get_ID() << std::endl;
            std::cout << "Nume produs: " << product.get_name() << std::endl;
            std::cout << "Pret " << product.get_price() << " RON" << std::endl;
            Product::print_stock_status(product);
        }
    }

    void list_postmans() const {
        std::cout<<"Curieri disponibili: "<<std::endl;
        int id=1;
        for (const auto & postman : list_of_postmans) {
            std::cout<<"ID: "<<id<<std::endl;
            std::cout<<"Nume: "<<postman.get_name()<<std::endl;
            std::cout<<"Tier: "<<postman.get_tier()<<std::endl;
            id++;
        }
    }
    void order_statuses() {
        for (auto& order : order_history) {
            if(order.status == OrderStatus::Procesare)
                order.status = OrderStatus::InTransit;
            else if (order.status == OrderStatus::InTransit)
                order.status = OrderStatus::Livrat;
        }
    }
    void view_order_history() {
        if (order_history.empty()) {
            std::cout << "Nu ai comenzi plasate inca.\n";
            return;
            }
        order_statuses();
        std::cout << "\nIstoricul comenzilor\n";
        for (const auto& order : order_history) order.print();
    }

    static std::string hash_password(const std::string& password) {
        std::string hash_hex_str;
        picosha2::hash256_hex_string(password, hash_hex_str);
        return hash_hex_str;
    }

    void admin_panel() {
        bool admin_loop = true;
        while (admin_loop) {
            std::cout << "Alege o optiune: "<<std::endl ;
            std::cout << "1. Adauga produs nou" << std::endl;
            std::cout << "2. Sterge produs" << std::endl;
            std::cout << "3. Editeaza produs" << std::endl;
            std::cout << "4. Afiseaza toate produsele" << std::endl;
            std::cout << "5. Inapoi la meniul principal" << std::endl;
            int choice = safe_read();

            switch (choice) {
                case 1: {
                    std::string name;
                    int stock, id, price;
                    float weight;
                    std::cout << "Nume produs: ";
                    std::cin >> std::ws;
                    std::getline(std::cin, name);

                    std::cout << "Numar in stoc: "; std::cin >> stock;
                    std::cout << "Greutate: "; std::cin >> weight;
                    std::cout << "ID Produs: "; std::cin >> id;
                    std::cout << "Pret: "; std::cin >> price;

                    products.emplace_back(name, stock, weight, id, price);
                    std::cout << "Produs adaugat cu succes!\n";
                    break;
                }
                case 2: {
                    int id_to_delete;
                    std::cout << "Introdu ID-ul produsului de sters: ";
                    std::cin >> id_to_delete;
                    auto it = std::remove_if(products.begin(), products.end(),[id_to_delete](const Product& p) { return p.get_ID() == id_to_delete; });
                    if (it != products.end()) {
                        products.erase(it, products.end());
                        std::cout << "Produs sters cu succes!\n";
                    } else {
                        std::cout << "Produsul cu acest ID nu exista.\n";
                    }
                    break;
                }
                case 3: {
                    int id_to_edit;
                    std::cout << "Introdu ID-ul produsului pe care vrei sa il editezi: ";
                    std::cin >> id_to_edit;
                    auto it = std::find_if(products.begin(), products.end(), [id_to_edit](const Product& p) { return p.get_ID() == id_to_edit; });
                    if (it != products.end()) {
                        std::cout << "Alege atributul de modificat\n";
                        std::cout << "1. Nume\n2. Stoc\n3. Greutate\n4. Pret\n";
                        std::cout << "Alege optiunea: ";
                        int edit_choice= safe_read();

                        switch (edit_choice) {
                            case 1: {
                                std::string new_name;
                                std::cout << "Introdu noul nume: ";
                                std::cin >> std::ws;
                                std::getline(std::cin, new_name);
                                it->set_name(new_name);
                                std::cout << "Nume actualizat!\n";
                                break;
                            }
                            case 2: {
                                int new_stock;
                                std::cout << "Introdu noul stoc: ";
                                std::cin >> new_stock;
                                it->set_stock_number(new_stock);
                                std::cout << "Stoc actualizat!\n";
                                break;
                            }
                            case 3: {
                                float new_weight;
                                std::cout << "Introdu noua greutate: ";
                                std::cin >> new_weight;
                                it->set_weight(new_weight);
                                std::cout << "Greutate actualizata!\n";
                                break;
                            }
                            case 4: {
                                int new_price;
                                std::cout << "Introdu noul pret: ";
                                std::cin >> new_price;
                                it->set_price(new_price);
                                std::cout << "Pret actualizat!\n";
                                break;
                            }
                            default:
                                std::cout << "Optiune invalida!\n";
                        }
                    } else {
                        std::cout << "Produsul cu acest ID nu exista.\n";
                    }
                    break;
                }

                case 4: {
                    list_products();
                    break;
                }
                case 5: {
                    admin_loop = false;
                    break;
                }
                default:
                    std::cout << "Optiune invalida!\n";
            }
        }
    };
    void user_panel() {
        bool loop = true;
        srand(time(nullptr));
        int id_product,id_postman;
        std::vector<std::string> package_content;
         std::cout << "Introduce-ti numele, adresa, nr de telefon si email-ul:\n";
            Destinatar utilizator;
            std::cin >> utilizator;
            std::cout << "Utilizator creat cu succes:\n" << utilizator;
            Colet package_box(package_content, 0, static_cast<int>(rand()), 0, utilizator);
            while (loop) {
                std::cout << "\nChoose an option: " << std::endl;
                std::cout << "1: Listeaza produsele" << std::endl;
                std::cout << "2: Adauga produsul in cos" << std::endl;
                std::cout << "3: Sterge produs din cos" << std::endl;
                std::cout << "4: Afiseaza informatii colet" << std::endl;
                std::cout << "5: Trimite comanda" << std::endl;
                std::cout << "6: Istoric comenzi" << std::endl;
                std::cout << "7: Exit app" << std::endl;
                int choose=safe_read();

                switch (choose) {
                    case 1: {
                        list_products();
                        break;
                    }
                    case 2: {
                        std::cout << "Alege ID-ul produsului dorit: ";
                        std::cin >> id_product;
                        auto it = std::find_if(products.begin(), products.end(), [id_product](const Product& p) { return p.get_ID() == id_product; });
                        if (it != products.end()) {
                            if (package_box.add_item_into_box(*it))
                                std::cout << "Produs adaugat in cos!" << std::endl;
                        } else {
                            std::cout << "ID invalid!" << std::endl;
                        }
                        break;
                    }
                    case 3: {
                        std::cout << "Alege ID-ul produsului pe care vrei sa il stergi: ";
                        std::cin >> id_product;
                        auto it = std::find_if(products.begin(), products.end(), [id_product](const Product& p) { return p.get_ID() == id_product; });
                        if (it != products.end()) {
                            if (package_box.remove_item_from_box(*it)) {
                                std::cout << "Produs sters din cos cu succes!" << std::endl;
                            } else {
                                std::cout << "Produsul nu se afla in cosul tau!" << std::endl;
                            }
                        } else {
                            std::cout << "ID invalid!" << std::endl;
                        }
                        break;
                    }
                    case 4: {
                        std::cout << package_box;
                        break;
                    }
                    case 5: {
                        bool order = false;
                        while (!order) {
                            list_postmans();
                            std::cout << "Alege curierul dorit: (0 pt anulare)";
                            std::cin >> id_postman;
                            if (id_postman==0) break;
                            if (id_postman < 1 || id_postman > static_cast<int>(list_of_postmans.size())) {
                                std::cout << "Id invalid\n";
                                continue;
                            }
                            order = list_of_postmans[id_postman - 1].receive_box(package_box);

                            if (order) {
                                order_history.push_back({ next_order_id++, list_of_postmans[id_postman - 1].get_name(), package_box.package_price(), package_box.get_weight(), OrderStatus::Procesare });
                                std::cout << "Comanda #" << (next_order_id - 1) << " plasata cu succes!\n";
                                package_box.reset(static_cast<int>(rand()));
                            }
                        }
                        break;
                    }
                    case 6: {
                        view_order_history();
                        break;
                    }

                    case 7: {
                        loop = false;
                        break;
                    }
                    default: {
                        std::cout << "Optiune invalida\n";
                        break;
                    }
                }
            }
        }



public:
    FancurierApp() =default;


    void init() {
        bool login_loop=true;
        std::string password;
        while (login_loop) {
            std::cout << "Welcome to FanCurier. Creaza-ti un cont!" << std::endl;
            std::cout<<"Alege:'\n' 1. Register '\n' 2. Admin Login '\n' 3. Exit"<<std::endl;
            int login_param=safe_read();
            switch (login_param) {
                case 1:{
                    user_panel();
                    break;
                }
                case 2: {
                    std::cout<<"Introdu Parola:"<<std::endl;
                    std::cin>>password;
                    const std::string ADMIN_HASH = "2ec8e5414c6351fb9c23e9e82a2faca3015ef1938d0579cd8c43c8bc18efa02f";
                    if (hash_password(password)==ADMIN_HASH) {
                        admin_panel();
                    }
                    else {
                        std::cout<<"Access interzis."<<std::endl;
                    }
                    break;
                }
                case 3: {
                    login_loop=false;
                    break;
                }
                default: {
                    std::cout<<"Optiune invalida ";
                    break;
                }
            }
        }
    }
};

int main() {
    FancurierApp app;
    app.init();
    return 0;
}



