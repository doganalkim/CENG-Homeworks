import javax.naming.ContextNotEmptyException;
import java.util.ArrayList;

public class CengTreeNodeInternal extends CengTreeNode
{
    private ArrayList<Integer> keys;
    private ArrayList<CengTreeNode> children;

    public CengTreeNodeInternal(CengTreeNode parent)
    {
        super(parent);

        // TODO: Extra initializations, if necessary.
        this.keys = new ArrayList<Integer>();
        this.children = new ArrayList<CengTreeNode>();
    }

    // GUI Methods - Do not modify
    public ArrayList<CengTreeNode> getAllChildren()
    {
        return this.children;
    }
    public Integer keyCount()
    {
        return this.keys.size();
    }
    public Integer keyAtIndex(Integer index)
    {
        if(index >= this.keyCount() || index < 0)
        {
            return -1;
        }
        else
        {
            return this.keys.get(index);
        }
    }

    // Extra Functions
    public ArrayList<Integer> getAllKeys(){
        return this.keys;
    }

    public void setKeys(ArrayList<Integer> k){
        this.keys = k;
    }

    public void setChildren(ArrayList<CengTreeNode> c){
        this.children = c;
    }

    public void addChild(CengTreeNode n){
        this.children.add(n);
    }

    public void insert_key_and_pointer(Integer key, CengTreeNode new_node){
        int i  = 0;
        int sz = ((CengTreeNodeInternal)this).keyCount();
        while(i < sz && key >= ((CengTreeNodeInternal)this).keyAtIndex(i)){
            //System.out.println("insert_key_and_pointer called");
            i++;
        }
        //System.out.println("index:"+ i + " the size:" + sz );
        this.keys.add(i,key);
        this.children.add(i+1,new_node);
        if( this.keys.size() > 2*this.order){
            this.split();
        }
    }

}
