import java.lang.reflect.Array;
import java.util.ArrayList;

public class CengTree
{
    public CengTreeNode root;
    // Any extra attributes...

    public CengTree(Integer order)
    {
        CengTreeNode.order = order;
        // TODO: Initialize the class
        this.root = new CengTreeNodeLeaf(null);
    }

    public void addBook(CengBook book)
    {
        // TODO: Insert Book to Tree
        this.root.insert_helper(book);
        this.change_root();
    }

    public ArrayList<CengTreeNode> searchBook(Integer bookID)
    {
        // TODO: Search within whole Tree, return visited nodes.
        // Return null if not found.
        String id = bookID.toString();
        if(this.root == null){
            System.out.println("Could not find " + id + ".");
            return null;
        }
        ArrayList<CengTreeNode> res = new ArrayList<>();

        CengTreeNode node = this.root;

        while(node.getType() == CengNodeType.Internal){
            res.add(node);
            Integer i = 0;
            Integer len = ((CengTreeNodeInternal)node).keyCount();
            while(i < len && ((CengTreeNodeInternal)node).keyAtIndex(i) <= bookID){
                i++;
            }

            node = ((CengTreeNodeInternal)node).getAllChildren().get(i);
        }
        res.add(node);

        Integer i = 0;
        Integer len = ((CengTreeNodeLeaf)node).bookCount();
        while( i < len ){
            if( ((CengTreeNodeLeaf)node).bookKeyAtIndex(i) == bookID )
            {
                break;
            }
            i++;
        }
        if( i == len){
            System.out.println("Could not find " + id + ".");
            return null;
        }

        Integer n = res.size();
        String t = "";
        for(int ind = 0; ind < n - 1 ; ind++){
            System.out.println(t + "<index>");
            int size = ((CengTreeNodeInternal)res.get(ind)).keyCount();
            for(int k = 0 ; k < size ; k++){
                System.out.println(t + ((CengTreeNodeInternal)res.get(ind)).keyAtIndex(k));
            }
            System.out.println(t + "</index>");
            t = t + "\t";
        }

        ArrayList<CengBook>  leafBooks = ((CengTreeNodeLeaf)res.get(n-1)).getBooks();
        n = leafBooks.size();
        System.out.println(t + "<data>");
        for(int ind = 0; ind < n ; ind++){
            Integer aydi = leafBooks.get(ind).getBookID();
            String booktitle = leafBooks.get(ind).getBookTitle();
            String author = leafBooks.get(ind).getAuthor();
            String genre = leafBooks.get(ind).getGenre();
            System.out.println(t + "<record>"+ aydi.toString() +"|"+ booktitle+"|"+author+"|"+genre+"</record>");
        }
        System.out.println(t+"</data>");
        /*
        CengBook book = ((CengTreeNodeLeaf)node).getBookAtIndex(i);
        String bookTitle = book.getBookTitle();
        String author = book.getAuthor();
        String genre = book.getGenre();
        System.out.println(t + "<record>"+id+"|"+bookTitle+"|"+author+"|"+genre+"</record>");
        */

        return res;
    }

    public void printTree()
    {
        // TODO: Print the whole tree to console
        if(this.root == null){
            return;
        }
        this.root.printHelper("");
    }

    // Any extra functions...

    public void change_root(){
        while(this.root.getParent() != null){
            this.root = root.getParent();
        }
    }



}
