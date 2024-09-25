package ech.android.smartshopping;

public class Items {
    int Price, Qty;

    public Items() {

    }

    public Items(int price, int qty) {
        Price = price;
        Qty = qty;
    }

    public int getPrice() {
        return Price;
    }

    public int getQty() {
        return Qty;
    }
}
