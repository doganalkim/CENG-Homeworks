import java.util.ArrayList;

public class CengTreeNodeLeaf extends CengTreeNode
{
    private ArrayList<CengBook> books;
    // TODO: Any extra attributes

    public CengTreeNodeLeaf(CengTreeNode parent)
    {
        super(parent);

        // TODO: Extra initializations
        books = new ArrayList<CengBook>();
    }

    // GUI Methods - Do not modify
    public int bookCount()
    {
        return books.size();
    }
    public Integer bookKeyAtIndex(Integer index)
    {
        if(index >= this.bookCount()) {
            return -1;
        } else {
            CengBook book = this.books.get(index);

            return book.getBookID();
        }
    }

    // Extra Functions
    public ArrayList<CengBook> getBooks(){
        return this.books;
    }

    public void setBooks(ArrayList<CengBook> b){
        this.books = b;
    }

    public CengBook getBookAtIndex(Integer index){
        return this.books.get(index);
    }
}
