import java.awt.Color;
import java.util.ArrayList;
import java.util.List;

public abstract class CengTreeNode
{
    static protected Integer order;
    private CengTreeNode parent;
    protected CengNodeType type; // Type needs to be set for proper GUI. Check CengNodeType.java.

    // GUI Accessors - do not modify
    public Integer level;
    public Color color;

    // Any extra attributes, if necessary

    public CengTreeNode(CengTreeNode parent)
    {
        this.parent = parent;

        this.color = CengGUI.getRandomBorderColor();

        // TODO: Extra initializations, if necessary.
        this.type = CengNodeType.Leaf;
    }

    // Getters-Setters - Do not modify
    public CengTreeNode getParent()
    {
        return parent;
    }

    public void setParent(CengTreeNode parent)
    {
        this.parent = parent;
    }

    public CengNodeType getType()
    {
        return type;
    }

    // GUI Methods - Do not modify
    public Color getColor()
    {
        return this.color;
    }

    // Extra Functions

    public void setType(){
        this.type = CengNodeType.Internal;
    }

    public void setOrder(int o){
        this.order = o;
    }


    public void printHelper(String t){
        if(this.getType() == CengNodeType.Leaf ){
            System.out.println(t + "<data>");
            for(int i  = 0; i < ((CengTreeNodeLeaf)this).bookCount() ; i++){
                CengBook book = ((CengTreeNodeLeaf)this).getBookAtIndex(i);
                String id = book.getBookID().toString();
                String bookTitle = book.getBookTitle();
                String author = book.getAuthor();
                String genre = book.getGenre();
                System.out.println(t +"<record>"+id+"|"+bookTitle+"|"+author+"|"+genre+"</record>");
            }
            System.out.println(t + "</data>");
        }
        else{
            System.out.println(t + "<index>");
            for(int i = 0; i < ((CengTreeNodeInternal)this).keyCount() ; i++){
                System.out.println( t + ((CengTreeNodeInternal)this).keyAtIndex(i));
            }
            System.out.println(t + "</index>");
            ArrayList<CengTreeNode> children = ((CengTreeNodeInternal)this).getAllChildren();
            int size = children.size();
            for(int i  = 0; i < size ; i++ ){
                children.get(i).printHelper(t + "\t");
            }
        }
    }

    public void insert_helper(CengBook b){
        if(this.getType() == CengNodeType.Leaf ){
            int i = 0;
            int size = ((CengTreeNodeLeaf)this).getBooks().size();
            while(i < size && b.getBookID() >= ((CengTreeNodeLeaf)this).getBooks().get(i).getBookID() ){
                i++;
            }
            ((CengTreeNodeLeaf)this).getBooks().add(i,b);
            if(((CengTreeNodeLeaf)this).getBooks().size() > 2*this.order){
                this.split();
            }
        }
        else{
            int i = 0;
            int size = ((CengTreeNodeInternal)this).keyCount();
            while(i < size && b.getBookID() >= ((CengTreeNodeInternal)this).keyAtIndex(i)){
                i++;
            }
            ((CengTreeNodeInternal)this).getAllChildren().get(i).insert_helper(b);
        }
    }

    public void split(){
        if(this.getType() == CengNodeType.Leaf){
            List<CengBook> firstArr = ((CengTreeNodeLeaf)this).getBooks().subList(0,this.order);
            List<CengBook> secondArr = ((CengTreeNodeLeaf)this).getBooks().subList(this.order,2*this.order + 1);
            if(this.parent == null){
                this.parent = new CengTreeNodeInternal(null);
                this.parent.setType();
                this.parent.setOrder(this.order);
                ((CengTreeNodeInternal)this.getParent()).addChild(this);
            }
            CengTreeNodeLeaf newNode = new CengTreeNodeLeaf(this.getParent());
            newNode.setParent(this.getParent());
            ((CengTreeNodeLeaf) this).setBooks(new ArrayList<>(firstArr));
            newNode.setBooks(new ArrayList<>(secondArr));
            ((CengTreeNodeInternal)this.getParent()).insert_key_and_pointer(secondArr.get(0).getBookID(),newNode);

        }
        else{
            if(this.parent == null){
                this.parent = new CengTreeNodeInternal(null);
                this.parent.setType();
                this.parent.setOrder(this.order);
                ((CengTreeNodeInternal)this.getParent()).addChild(this);
            }
            CengTreeNodeInternal newNode = new CengTreeNodeInternal(this.getParent());
            newNode.setType();
            List<Integer> keyFirstArr = ((CengTreeNodeInternal)this).getAllKeys().subList(0,this.order);
            List<Integer> keySecondArr = ((CengTreeNodeInternal)this).getAllKeys().subList(this.order+1,2*this.order+1);
            Integer push_up_value = ((CengTreeNodeInternal)this).keyAtIndex(this.order);
            List<CengTreeNode> childFirstArr = ((CengTreeNodeInternal)this).getAllChildren().subList(0,this.order+1);
            List<CengTreeNode> childSecondArr = ((CengTreeNodeInternal)this).getAllChildren().subList(this.order+1,2*this.order+2);
            ((CengTreeNodeInternal)this).setKeys(new ArrayList<>(keyFirstArr));
            newNode.setKeys(new ArrayList<>(keySecondArr));
            ((CengTreeNodeInternal)this).setChildren(new ArrayList<>(childFirstArr));
            newNode.setChildren(new ArrayList<>(childSecondArr));
            for(int i = 0 ; i <  newNode.getAllChildren().size()  ; i++){
                ((CengTreeNodeInternal)newNode).getAllChildren().get(i).setParent(newNode);
                //System.out.println( "i:" + i   );
            }
            ((CengTreeNodeInternal)this.getParent()).insert_key_and_pointer(push_up_value,newNode);
        }
    }

}
