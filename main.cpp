#include <iostream>
#include <string>
#include <regex>
#include <utility>
#include <vector>
#include <cstdlib>

class Product {
    std::string name;
    int stock_number;
    int weight;
    int ID;
    int price;

public:
    Product(std::string product_name, int product_stock_number, int product_weight, int product_id,
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
    [[nodiscard]] int get_weight() const { return weight; }
    [[nodiscard]] int get_ID() const { return ID; }
    [[nodiscard]] int get_price() const { return price; }

    void scade_stock() {
        if (stock_number > 0)
            stock_number--;
    }

    static bool stock_status(const Product &product) {
        if (product.stock_number > 1) {
            std::cout << "In stoc!" << std::endl;
            return true;
        }
        if (product.stock_number == 1) {
            std::cout << "Un singur produs in stoc!";
            return true;
        }
        std::cout << "Stoc epuizat!" << std::endl;
        return false;
    }
};

class Destinatar {
    std::string name;
    std::string address;
    std::string email;
    std::string phone_number;

    static bool validate_phone_number(const std::string &nr_tel) {
        if (nr_tel.length() != 10 || nr_tel[0] != '0' || nr_tel[1] != '7') {
            std::cout << "Nr de telefon invalid! ";
            return false;
        }
        return true;
    }

    static bool validate_email(const std::string &email) {
        //inspo https://clehaxze.tw/gemlog/2023/01-28-validate-email-address-using-regex-in-cpp.gmi
        static const std::regex email_regex(
            R"(^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$)");
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
    int weight;
    const int AWB;
    int price;

public:
    [[nodiscard]] int package_price() const {
        return price;
    }

private:
    Destinatar destinatar;

public:
    Colet(std::vector<std::string> box_content, int box_weight, int box_awb, int box_price,
          Destinatar destinatar_colet) : content(std::move(box_content)), weight(box_weight), AWB(box_awb),
                                         price(box_price), destinatar(std::move(destinatar_colet)) {
    }

    [[nodiscard]] int get_weight() const {
        return weight;
    }

    friend std::ostream &operator<<(std::ostream &os, const Colet &colet) {
        for (const auto& item:colet.content)
            os << "Content: " <<item<<std::endl;
        os << "AWB: " << colet.AWB << std::endl;
        os << "Weight: " << colet.weight << std::endl;
        os << "Price: " << colet.price << std::endl;
        os << "Date destinatar: " << colet.destinatar << std::endl;
        return os;
    }

    void add_item_into_box(Product &chosen_product) {
        this->price += chosen_product.get_price();
        this->weight += chosen_product.get_weight();
        this->content.push_back(chosen_product.get_name());
        chosen_product.scade_stock();
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

};

class Curier {
    const std::string phone_number;
    const std::string name;
    int tier;
    Duba duba;

public:
    Curier(std::string numar_telefon_curier, std::string nume_curier, int tier,Duba duba_curier) : phone_number(std::move(numar_telefon_curier)), name(std::move(nume_curier)),tier(tier), duba(std::move(duba_curier)) {}

    void receive_box(const Colet& package) {
        double multiplier = 1.0 + (this->tier * 0.10);
        int actual_capacity = this->duba.get_max_capacity() * multiplier;
        if (package.get_weight()<=actual_capacity) {
            std::cout<<"Coletul va fi livrat de: "<<this->name << std::endl;
            std::cout<<"Contact: "<<this->phone_number<<" "<<this->duba.get_numar_inmatriculare()<<std::endl;
        }
        else {
            std::cout<<"Capacity excedeed! Choose another postman."<<std::endl;
        }
        if (package.package_price() > 2000 && this->tier <= 2) {
            std::cout << "Eroare: Comenzile peste 2000 RON necesita un curier VIP (Tier 3+)!\n";
            return;
        }
    }

    [[nodiscard]] const std::string& get_name() const {
        return name;
    }

    [[nodiscard]] int get_tier() const {
        return tier;
    }
};


static void list_products(const std::vector<Product> &products) {
    std::cout << "Lista de produse disponibile astazi: " << std::endl;
    for (const auto & product : products) {
        std::cout << "ID: " << product.get_ID() << std::endl;
        std::cout << "Nume produs: " << product.get_name() << std::endl;
        std::cout << "Pret " << product.get_price() << " RON" << std::endl;
        Product::stock_status(product);
    }
}

static void make_order(const std::vector<Curier> &postmans) {
    std::cout<<"Curieri disponibili: "<<std::endl;
    int id=1;
    for (const auto & postman : postmans) {
        std::cout<<"ID: "<<id<<std::endl;
        std::cout<<"Nume: "<<postman.get_name()<<std::endl;
        std::cout<<"Tier: "<<postman.get_tier()<<std::endl;
        id++;
    }

}


int main() {
    bool loop = true;
    int choose, id_product,id_postman;
    std::vector<std::string> package_content;

    srand(time(nullptr));
    std::cout << "Welcome to FanCurier. Creaza-ti un cont!" << std::endl;
    std::cout << "Introduce-ti numele, adresa, nr de telefon si email-ul:\n";
    Destinatar utilizator;
    std::cin >> utilizator;
    std::cout << "Utilizator creat cu succes:\n" << utilizator;
    Colet package_box(package_content, 0, int(rand()), 0, utilizator);


    std::vector<Product> products{
        Product("casti audio", 10, 300, 1, 100),
        Product("masina de spalat", 1, 1500, 2, 2000),
        Product("cafea", 20, 500, 3, 40),
        Product("deodorant", 30, 100, 4, 15),
        Product("matura", 34, 200, 5, 10)
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



    while (loop) {
        std::cout << "Choose an option: " << std::endl;
        std::cout<<"1: Listeaza produsele"<<std::endl;
        std::cout<<"2: Adauga produsul in cos"<<std::endl;
        std::cout<<"3: Afiseaza informatii colet"<<std::endl;
        std::cout<<"4: Trimite comanda"<<std::endl;
        std::cout<<"5: Exit app"<<std::endl;
        std::cin >> choose;
        switch (choose) {
            case 1: {
                list_products(products);
                break;
            }
            case 2: {
                std::cout << "Alege ID-ul produsului dorit:";
                std::cin >> id_product;
                if (id_product > 0 and id_product <= int(products.size())) {
                    Product &chosen_product = products[id_product - 1];
                    package_box.add_item_into_box(chosen_product);
                } else
                    std::cout << "Id invalid";
                break;
            }
            case 3: {
                std::cout<<package_box;
                break;
            }
            case 4: {
                make_order(list_of_postmans);
                std::cout << "Alege curierul dorit:";
                std::cin >> id_postman;
                if (id_postman > 0 and id_postman <= int(list_of_postmans.size())) {
                    list_of_postmans[id_postman - 1].receive_box(package_box);
                } else
                    std::cout << "Id invalid";
                break;
            }
            case 5: {
                loop = false;
                break;
            }
            default: {
                std::cout<<"Optiune invalida ";
                break;
            }
        }
    }

    return 0;
}



