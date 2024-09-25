package ech.android.smartshopping;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {

    TextView tv_item;
    Button btn_unload;
    FirebaseDatabase firebaseDatabase;
    DatabaseReference databaseReference;
    String billDisplay="";
    int total=0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tv_item = findViewById(R.id.tv_item);
        btn_unload = findViewById(R.id.btn_unload);
        firebaseDatabase = FirebaseDatabase.getInstance();
        databaseReference = firebaseDatabase.getReference("Cart1");

        btn_unload.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                databaseReference.removeValue();
                billDisplay="";
                total=0;
            }
        });

        // Read from the database
        databaseReference.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                for(DataSnapshot ds: dataSnapshot.getChildren()){
                    Items items = ds.getValue(Items.class);
                    if(items.getQty()!=0){
                        billDisplay = billDisplay+"\n\nItem: "+ds.getKey()+"\n\tPrice: Rs."+items.getPrice()+"\n\tQty: "+items.getQty();
                        total = total+ items.getPrice();
                    }
                }
                tv_item.setText("");
                tv_item.setText(billDisplay+"\n\n Grand Total: Rs "+total+" only.");
            }

            @Override
            public void onCancelled(DatabaseError error) {

            }
        });

    }
}