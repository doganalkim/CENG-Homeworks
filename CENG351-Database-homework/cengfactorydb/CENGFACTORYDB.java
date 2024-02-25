package ceng.ceng351.cengfactorydb;

public class CENGFACTORYDB implements ICENGFACTORYDB{
    /**
     * Place your initialization code inside if required.
     *
     * <p>
     * This function will be called before all other operations. If your implementation
     * need initialization , necessary operations should be done inside this function. For
     * example, you can set your connection to the database server inside this function.
     */
    private java.sql.Connection connection;
    public void initialize() {
        String url = "jdbc:mysql://144.122.71.128:8080/db2521482";
        String userName = "e2521482";
        String password = "&3H~f+Qdyzo$";
        connection = null;
        try {
            connection = java.sql.DriverManager.getConnection(url, userName, password);
            //System.out.println("Database connection established successfully.");
        } catch (java.sql.SQLException e) {
            e.printStackTrace();
            //System.err.println("Error connecting to the database!");
        }
    }

    /**
     * Should create the necessary tables when called.
     *
     * @return the number of tables that are created successfully.
     */
    public int createTables(){
        //System.out.println("CALLED createTables");
        String var = "CREATE TABLE Shipment (factoryId INT, productId INT, amount INT, pricePerUnit INT, PRIMARY KEY(factoryId,productId) , FOREIGN KEY(factoryId, productId) REFERENCES Produce(factoryId,productId) ON DELETE CASCADE ON UPDATE CASCADE  )";
        String var1 = "CREATE TABLE Shipment (factoryId INT, productId INT, amount INT, pricePerUnit INT, PRIMARY KEY(factoryId,productId), FOREIGN KEY (factoryId) REFERENCES Factory  ON DELETE CASCADE ON UPDATE CASCADE , FOREIGN KEY(productId) REFERENCES Product  ON DELETE CASCADE  ON UPDATE CASCADE)";
        String[] queries = {"CREATE TABLE Factory  (factoryId INT, factoryName TEXT, factoryType TEXT, country TEXT, PRIMARY KEY (factoryId))",
                "CREATE TABLE Employee (employeeId INT, employeeName TEXT, department TEXT, salary INT, PRIMARY KEY (employeeId))",
                "CREATE TABLE Works_In (factoryId INT, employeeId INT, startDate DATE, PRIMARY KEY(factoryId, employeeId), FOREIGN KEY (factoryId) REFERENCES Factory ON DELETE CASCADE ON UPDATE CASCADE  , FOREIGN KEY (employeeId) REFERENCES Employee   ON DELETE CASCADE ON UPDATE CASCADE )",
                "CREATE TABLE Product (productId INT, productName TEXT, productType TEXT, PRIMARY KEY(productId))",
                "CREATE TABLE Produce (factoryId INT, productId INT, amount INT, productionCost INT, PRIMARY KEY(factoryId, productId) , FOREIGN KEY (factoryId) REFERENCES Factory  ON DELETE CASCADE ON UPDATE CASCADE , FOREIGN KEY(productId) REFERENCES Product  ON DELETE CASCADE  ON UPDATE CASCADE)",
                var1};
        int result = 0;
        for (String query : queries) {
            try {
                java.sql.Statement statement = connection.createStatement();
                statement.executeUpdate((query));
                result++;
            } catch (java.sql.SQLException e) {
                e.printStackTrace();
            }
        }
        return result;
    }

    /**
     * Should drop the tables if exists when called.
     *
     * @return the number of tables are dropped successfully.
     */
    public int dropTables() {
        String[] queries = {
                "DROP TABLE IF EXISTS Shipment",
                "DROP TABLE IF EXISTS Works_In",
                "DROP TABLE IF EXISTS Produce",
                "DROP TABLE IF EXISTS Factory",
                "DROP TABLE IF EXISTS Employee",
                "DROP TABLE IF EXISTS Product",
        };
        int result = 0;
        for(String query: queries){
            try{
                java.sql.Statement statement = connection.createStatement();
                statement.executeUpdate(query);
                result++;
            }
            catch (java.sql.SQLException e){
                e.printStackTrace();
            }
        }
        return result;
    }

    /**
     * Should insert an array of Factory into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertFactory(Factory[] factories) {
        String query = "INSERT INTO Factory VALUES(?,?,?,?)";
        //System.out.println("CALLED insertFactory");
        //System.out.println(query);
        int result = 0;
        for(Factory factory:factories){
            try{
                java.sql.PreparedStatement preparedStatement= connection.prepareStatement(query);
                preparedStatement.setInt(1, factory.getFactoryId());
                preparedStatement.setString(2, factory.getFactoryName());
                preparedStatement.setString(3, factory.getFactoryType());
                preparedStatement.setString(4, factory.getCountry());
                preparedStatement.executeUpdate();
                //System.out.println(preparedStatement);
                result++;
            }
            catch (java.sql.SQLException e){
                e.printStackTrace();
            }
        }
        return result;
    }
    /**
     * Should insert an array of Employee into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertEmployee(Employee[] employees) {
        String query = "INSERT INTO  Employee  VALUES(?,?,?,?)";
        //System.out.println("CALLED insertEmployee");
        //System.out.println(query);
        int result = 0;
        for(Employee e:employees){
            try{
                java.sql.PreparedStatement preparedStatement= connection.prepareStatement(query);
                preparedStatement.setInt(1, e.getEmployeeId());
                preparedStatement.setString(2, e.getEmployeeName());
                preparedStatement.setString(3, e.getDepartment());
                preparedStatement.setInt(4, e.getSalary());
                preparedStatement.executeUpdate();
                //System.out.println(preparedStatement);
                result++;
            }
            catch (java.sql.SQLException E){
                E.printStackTrace();
            }
        }
        return result;
    }

    /**
     * Should insert an array of WorksIn into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertWorksIn(WorksIn[] worksIns) {
        String query = "INSERT INTO  Works_In  VALUES(?,?,?)";
        //System.out.println("CALLED insertWorksIn");
        //System.out.println(query);
        int result = 0;
        for(WorksIn w: worksIns){
            try{
                java.sql.PreparedStatement preparedStatement= connection.prepareStatement(query);
                preparedStatement.setInt(2, w.getEmployeeId());
                preparedStatement.setInt(1, w.getFactoryId());
                preparedStatement.setString(3, w.getStartDate());
                preparedStatement.executeUpdate();
                //System.out.println(preparedStatement);
                result++;
            }
            catch (java.sql.SQLException e){
                e.printStackTrace();
            }
        }
        return result;
    }


    /**
     * Should insert an array of Product into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduct(Product[] products) {
        String query = "INSERT INTO Product VALUES(?,?,?)";
        //System.out.println("CALLED insertProduct");
        //System.out.println(query);
        int result = 0;

        for(Product p: products){
            try{
                java.sql.PreparedStatement preparedStatement= connection.prepareStatement(query);
                preparedStatement.setInt(1, p.getProductId());
                preparedStatement.setString(2, p.getProductName());
                preparedStatement.setString(3, p.getProductType());
                preparedStatement.executeUpdate();
                //System.out.println(preparedStatement);
                result++;
            }
            catch(java.sql.SQLException e){
                e.printStackTrace();
            }
        }
        return result;
    }

    /**
     * Should insert an array of Produce into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertProduce(Produce[] produces) {
        String query = "INSERT INTO Produce VALUES(?,?,?,?)";
        //System.out.println("CALLED insertProduce");
        //System.out.println(query);
        int result = 0;
        for(Produce p: produces){
            try{
                java.sql.PreparedStatement preparedStatement= connection.prepareStatement(query);
                preparedStatement.setInt(1, p.getFactoryId());
                preparedStatement.setInt(2, p.getProductId());
                preparedStatement.setInt(3, p.getAmount());
                preparedStatement.setInt(4, p.getProductionCost());
                preparedStatement.executeUpdate();
                result++;
            }
            catch (java.sql.SQLException e){
                e.printStackTrace();
            }
        }
        return result;
    }

    /**
     * Should insert an array of Shipment into the database.
     *
     * @return Number of rows inserted successfully.
     */
    public int insertShipment(Shipment[] shipments) {
        String query = "INSERT INTO Shipment VALUES (?,?,?,?) ";
        //System.out.println("CALLED insertShipment");
        //System.out.println(query);
        int result = 0;
        for(Shipment s:shipments){
            try{
                java.sql.PreparedStatement preparedStatement= connection.prepareStatement(query);
                preparedStatement.setInt(1, s.getFactoryId());
                preparedStatement.setInt(2, s.getProductId());
                preparedStatement.setInt(3, s.getAmount());
                preparedStatement.setInt(4, s.getPricePerUnit());
                preparedStatement.execute();
                result++;
            }
            catch (java.sql.SQLException e){
                e.printStackTrace();
            }

        }
        return result;
    }
    /**
     * Should return all factories that are located in a particular country.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesForGivenCountry(String country) {
        //System.out.println("CALLED getFactoriesForGivenCountry");
        String query="SELECT DISTINCT * FROM Factory F WHERE F.country = +" + "'" + country + "' ORDER BY F.factoryId ASC";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query);
            //preparedStatement.setString(1,country);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            java.util.ArrayList<Factory> al = new java.util.ArrayList<>();
            while(res.next()){
                Factory f = new Factory(res.getInt("factoryId"),res.getString("factoryName"),res.getString("factoryType"),res.getString("country"));
                al.add(f);
            }
            int size = al.size();
            Factory[] ans = new Factory[size];
            int i = 0;
            for(Factory f : al){
                ans[i] = f;
                i++;
            }
            return ans;
        }
        catch (java.sql.SQLException e){
            e.printStackTrace();
        }
        return new Factory[0];
    }




    /**
     * Should return all factories without any working employees.
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesWithoutAnyEmployee() {
        //System.out.println("CALLED getFactoriesWithoutAnyEmployee");
        String query="SELECT DISTINCT * " +
                "FROM Factory F" +
                " WHERE NOT EXISTS ( SELECT *" +
                "                    FROM Works_In W " +
                "                    WHERE W.factoryId = F.factoryId " +
                "                    ) ORDER BY F.factoryId ASC ";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            java.util.ArrayList<Factory> al = new java.util.ArrayList<>();
            while(res.next()){
                Factory f = new Factory(res.getInt("factoryId"),res.getString("factoryName"),res.getString("factoryType"),res.getString("country"));
                al.add(f);
            }
            int size = al.size();
            Factory[] ans = new Factory[size];
            int i = 0;
            for(Factory f : al){
                ans[i] = f;
                i++;
            }
            return ans;
        }
        catch (java.sql.SQLException e){
            e.printStackTrace();
        }
        return new Factory[0];
    }

    /**
     * Should return all factories that produce all products for a particular productType
     *
     * @return Factory[]
     */
    public Factory[] getFactoriesProducingAllForGivenType(String productType) {
        //System.out.println("CALLED getFactoriesProducingAllForGivenType");
        String query="SELECT DISTINCT * " +
                "FROM Factory F " +
                "WHERE NOT EXISTS " +
                "( " +
                "    SELECT productId " +
                "    FROM Product " +
                "    WHERE productType = ? " +
                "    EXCEPT " +
                "    SELECT P.productId " +
                "    FROM Produce P " +
                "    WHERE P.factoryId = F.factoryId " +
                ") ORDER BY F.factoryId ASC ";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query);
            preparedStatement.setString(1,productType);
            //preparedStatement.setString(2,productType);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            java.util.ArrayList<Factory> al = new java.util.ArrayList<>();
            while(res.next()){
                Factory f = new Factory(res.getInt("factoryId"),res.getString("factoryName"),res.getString("factoryType"),res.getString("country"));
                al.add(f);
            }
            int size = al.size();
            Factory[] ans = new Factory[size];
            int i = 0;
            for(Factory f : al){
                ans[i] = f;
                i++;
            }
            return ans;
        }
        catch (java.sql.SQLException e){
            e.printStackTrace();
        }
        return new Factory[0];
    }


    /**
     * Should return the products that are produced in a particular factory but
     * don’t have any shipment from that factory.
     *
     * @return Product[]
     */
    public Product[] getProductsProducedNotShipped() {
        //System.out.println("CALLED getProductsProducedNotShipped");
        String query = "SELECT DISTINCT  Pr.* " +
                "FROM Produce P, Product Pr  " +
                "WHERE P.productId = Pr.productId AND NOT EXISTS " +
                "( " +
                "SELECT * " +
                "FROM Shipment S " +
                "WHERE S.productId = P.productId AND S.factoryId = P.factoryId " +
                ") ORDER BY P.productId ASC";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            java.util.ArrayList<Product> al = new java.util.ArrayList<>();
            while(res.next()){
                Product p = new Product(res.getInt("productId"),res.getString("productName"),res.getString("productType"));
                al.add(p);
            }
            int size = al.size();
            Product[] ans = new Product[size];
            int i = 0;
            for(Product p : al){
                ans[i] = p;
                i++;
            }
            return ans;
        }
        catch (java.sql.SQLException e){
            e.printStackTrace();
        }
        return new Product[0];
    }



    /**
     * For a given factoryId and department, should return the average salary of
     *     the employees working in that factory and that specific department.
     *
     * @return double
     */
    public double getAverageSalaryForFactoryDepartment(int factoryId, String department) {
        //System.out.println("CALLED getAverageSalaryForFactoryDepartment");
        String query = "SELECT AVG(E.salary) AS avgsalary " +
                "FROM Employee E, Works_In W " +
                "WHERE E.employeeId = W.employeeId AND W.factoryId = ?  AND E.department = ? ";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query);
            preparedStatement.setInt(1,factoryId);
            preparedStatement.setString(2,department);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            if(res.next())
                return res.getDouble("avgsalary") ;
            return 0.0;
        }
        catch(java.sql.SQLException e){
            e.printStackTrace();
        }
        return 0.0;
    }


    /**
     * Should return the most profitable products for each factory
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProducts() {
        //System.out.println("CALLED getMostValueableProducts");

        String query5 =
                " SELECT DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType, (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS profit " +
                        "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId JOIN Shipment S ON P.factoryId = S.factoryId AND P.productId = S.productID JOIN " +
                        "(" +
                            " SELECT U.factoryId AS factoryId, MAX(pro) AS pro " +
                            " FROM (  " +
                            "SELECT P.factoryId AS factoryId , (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS pro  " +
                            "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId JOIN Shipment S ON P.factoryId = S.factoryId AND P.productId = S.productID " +
                            "UNION " +
                            "SELECT P.factoryId AS factoryId, (-P.amount*P.productionCost) AS pro " +
                            "FROM  Produce P JOIN Product Pr ON P.productId = Pr.productId " +
                            "  WHERE  (P.factoryId,P.productId ) NOT IN (SELECT factoryId, productId FROM Shipment )  " +
                            ") U " +
                        " GROUP BY U.factoryId " +
                 ") T ON T.factoryId = P.factoryId WHERE T.pro = S.amount*S.pricePerUnit - P.amount*P.productionCost " +
                "UNION " +
                "SELECT  DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType, ( - P.amount*P.productionCost) AS profit " +
                "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId  JOIN " +
                "(" +
                    " SELECT U.factoryId AS factoryId, MAX(pro) AS pro " +
                    " FROM (  " +
                        "SELECT P.factoryId AS factoryId , (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS pro  " +
                        "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId JOIN Shipment S ON P.factoryId = S.factoryId AND P.productId = S.productID " +
                        "UNION " +
                        "SELECT P.factoryId AS factoryId, (-P.amount*P.productionCost) AS pro " +
                        "FROM  Produce P JOIN Product Pr ON P.productId = Pr.productId " +
                        "  WHERE (P.factoryId,P.productId ) NOT IN (SELECT factoryId, productId FROM Shipment ) " +
                    ") U " +
                    " GROUP BY U.factoryId " +
                ") T ON T.factoryId = P.factoryId WHERE (  (P.factoryId,P.productId ) NOT IN (SELECT factoryId, productId FROM Shipment )  )  AND  (-P.amount*P.productionCost) >= T.pro " +
                "ORDER BY  profit DESC, factoryId ASC ";

        String query4 =
                "SELECT DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType,  IFNULL(S.amount*S.pricePerUnit,0) - P.amount*P.productionCost AS profit " +
                "FROM Produce P JOIN Product Pr ON P.productId  = Pr.productId   " +
                        "LEFT JOIN Shipment S ON P.factoryId = S.factoryId AND P.productId = S.productId " +
                        "JOIN   " +
                        "(" +
                        "SELECT P1.factoryId AS factoryId, MAX( IFNULL(S1.amount*S1.pricePerUnit,0) - P1.amount*P1.productionCost ) AS profit1 " +
                        "FROM Produce P1 JOIN Product Pr1 ON P1.productId = Pr1.productId " +
                        "LEFT JOIN Shipment S1 ON P1.factoryId = S1.factoryId AND P1.productId = S1.productId " +
                                " GROUP BY P1.factoryId " +
                        ") T ON T.factoryId =  P.factoryId " +
                 " WHERE  IFNULL(S.amount*S.pricePerUnit,0) - P.amount*P.productionCost >=  T.profit1 " +
                " ORDER BY  profit DESC, factoryId ASC ";

        String query3 = "SELECT DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType, (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS profit  " +
                "FROM  Produce P, Product Pr, Shipment  S, " +
                "( " +
                "SELECT U.factoryId  AS z , MAX(profit)  AS x " +
                "FROM ( " +
                    " SELECT P.factoryId AS factoryId , (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS profit  " +
                    " FROM Product Pr, Produce P, Shipment S " +
                    " WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND S.productId = P.productId " +
                    " AND   P.factoryId IN ( SELECT S.factoryId FROM  Shipment S ) " +
                    " UNION" +
                    " SELECT P.factoryId AS factoryId, ( - P.amount*P.productionCost) AS profit " +
                    " FROM Product Pr, Produce P " +
                    " WHERE Pr.productId = P.productId " +
                    " AND P.factoryId NOT IN ( SELECT S.factoryId FROM Shipment S ) " +
                    " ) U " +
                " GROUP BY U.factoryId  " +
                ") T " +
                "WHERE T.z =  P.factoryId AND P.factoryId = S.factoryId AND  P.productId = S.productId AND P.productId = Pr.productId " +
                "AND (S.amount*S.pricePerUnit - P.amount*P.productionCost)  >=  T.x " +
                "UNION " +
                " SELECT DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType, (- P.productionCost*P.amount ) AS profit " +
                " FROM  Produce P,Product Pr, " +
                "( " +
                "SELECT U.factoryId  AS z , MAX(profit)  AS x " +
                    "FROM ( " +
                    " SELECT P.factoryId, (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS profit  " +
                    " FROM Product Pr, Produce P, Shipment S " +
                    " WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND S.productId = P.productId " +
                    " AND   P.factoryId IN ( SELECT S.factoryId FROM  Shipment S ) " +
                    " UNION " +
                    " SELECT P.factoryId,  (- P.amount*P.productionCost) AS profit " +
                    " FROM Product Pr, Produce P " +
                    " WHERE Pr.productId = P.productId " +
                    " AND P.factoryId NOT IN ( SELECT S.factoryId FROM Shipment S ) " +
                    " ) U " +
                " GROUP BY factoryId " +
                ")  T " +
                " WHERE P.productId = Pr.productId AND P.factoryId = T.z AND P.factoryId NOT IN ( SELECT S.factoryId FROM Shipment S ) AND (- P.productionCost*P.amount) >= T.x " +
                "ORDER BY profit DESC, factoryId ASC ";
        String query2 = "SELECT DISTINCT P.factoryId, Pr.productId, Pr.productName, Pr.productType, S.amount*S.pricePerUnit - P.amount*P.productionCost AS profit " +
                "FROM Product Pr, Produce P, Shipment S, " +
                "(" +
                "SELECT P.factoryId AS first, MAX(S.amount*S.pricePerUnit - P.amount*P.productionCost) AS second " +
                "FROM Product Pr, Produce P, Shipment S " +
                "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND Pr.productId = S.productId " +
                "GROUP BY P.factoryId" +
                ") T " +
                "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND Pr.productId = S.productId AND T.first = P.factoryId AND " +
                " S.amount*S.pricePerUnit - P.amount*P.productionCost >= T.second " +
                "UNION " +
                "SELECT P.factoryId, Pr.productId, Pr.productName, Pr.productType, - P.amount*P.productionCost AS profit " +
                "FROM Product Pr, Produce P, " +
                "(" +
                "SELECT DISTINCT  P.factoryId AS first, MAX(-P.amount*P.productionCost) AS second  " +
                "FROM Product Pr, Produce P " +
                "WHERE Pr.productId = P.productId AND P.factoryId NOT IN ( SELECT S.factoryId FROM Shipment S ) " +
                "GROUP BY P.factoryId " +
                ") T " +
                "WHERE Pr.productId = P.productId AND T.first = P.factoryId AND -P.amount*P.productionCost >= T.second " +
                "ORDER BY profit DESC, factoryId ASC ";
        String query = "SELECT DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType, (S.amount * S.pricePerUnit - P.amount * P.productionCost) AS profit " +
                "FROM  Produce P, Shipment S, Product Pr, " +
                "( " +
                "SELECT P.factoryId  AS z , MAX(S.amount * S.pricePerUnit - P.amount *P.productionCost)  AS x " +
                "FROM   Produce P, Shipment S " +
                "WHERE  P.factoryId = S.factoryId AND P.productId = S.productId " +
                "GROUP BY P.factoryId " +
                ")  T " +
                "WHERE T.z =  P.factoryId AND P.factoryId = S.factoryId AND  P.productId = S.productId AND P.productId = Pr.productId " +
                "AND  (S.amount*S.pricePerUnit - P.amount*P.productionCost)  >=  T.x " +
                "ORDER BY profit DESC, factoryId ASC ";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query5);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            java.util.ArrayList<QueryResult.MostValueableProduct> al = new java.util.ArrayList<>();
            while(res.next()){
                QueryResult.MostValueableProduct mvp = new  QueryResult.MostValueableProduct(res.getInt("factoryId"),
                        res.getInt("productId"),
                        res.getString("productName"),
                        res.getString("productType"),
                        res.getDouble("profit"));
                al.add(mvp);
            }
            int size = al.size();
            QueryResult.MostValueableProduct[] ans = new QueryResult.MostValueableProduct[size];
            int i = 0;
            for(QueryResult.MostValueableProduct mvp : al){
                ans[i] = mvp;
                i++;
            }
            return ans;
        }
        catch( java.sql.SQLException e){
            e.printStackTrace();
        }
        return new QueryResult.MostValueableProduct[0];
    }


    /**
     * For each product, return the factories that gather the highest profit
     * for that product
     *
     * @return QueryResult.MostValueableProduct[]
     */
    public QueryResult.MostValueableProduct[] getMostValueableProductsOnFactory() {

        String query3 =
                " SELECT DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType, (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS profit " +
                        "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId JOIN Shipment S ON P.factoryId = S.factoryId AND P.productId = S.productID JOIN " +
                        "(" +
                        " SELECT U.productId AS productId, MAX(pro) AS pro " +
                        " FROM (  " +
                        "SELECT P.productId AS productId , (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS pro  " +
                        "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId JOIN Shipment S ON P.factoryId = S.factoryId AND P.productId = S.productID " +
                        "UNION " +
                        "SELECT P.productId AS productId, (-P.amount*P.productionCost) AS pro " +
                        "FROM  Produce P JOIN Product Pr ON P.productId = Pr.productId " +
                        "  WHERE  P.productId  NOT IN (SELECT productId FROM Shipment )  " +
                        ") U " +
                        " GROUP BY U.productId " +
                        ") T ON T.productId = P.productId WHERE T.pro = S.amount*S.pricePerUnit - P.amount*P.productionCost " +
                        "UNION " +
                        "SELECT  DISTINCT P.factoryId, P.productId, Pr.productName, Pr.productType, ( - P.amount*P.productionCost) AS profit " +
                        "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId  JOIN " +
                        "(" +
                        " SELECT U.productId AS productId, MAX(pro) AS pro " +
                        " FROM (  " +
                        "SELECT P.productId AS productId , (S.amount*S.pricePerUnit - P.amount*P.productionCost) AS pro  " +
                        "FROM Produce P JOIN Product Pr ON P.productId = Pr.productId JOIN Shipment S ON P.factoryId = S.factoryId AND P.productId = S.productID " +
                        "UNION " +
                        "SELECT P.productId AS productId, (-P.amount*P.productionCost) AS pro " +
                        "FROM  Produce P JOIN Product Pr ON P.productId = Pr.productId " +
                        "  WHERE  P.productId  NOT IN (SELECT productId FROM Shipment )  " +
                        ") U " +
                        " GROUP BY U.productId " +
                        ") T ON T.productId = P.productId WHERE P.productId NOT IN (SELECT productId FROM Shipment )    AND  (-P.amount*P.productionCost) >= T.pro " +
                        "ORDER BY  profit DESC, productId ASC ";

        String query2 = "SELECT DISTINCT P.factoryId, Pr.productId, Pr.productName, Pr.productType, S.amount*S.pricePerUnit - P.amount*P.productionCost AS profit " +
                "FROM Product Pr, Produce P, Shipment S, " +
                "(" +
                "SELECT Pr.productId AS first, MAX(S.amount*S.pricePerUnit - P.amount*P.productionCost) AS second " +
                "FROM Product Pr, Produce P, Shipment S " +
                "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND Pr.productId = S.productId " +
                "GROUP BY Pr.productId" +
                ") T " +
                "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND Pr.productId = S.productId AND T.first = Pr.productId AND " +
                " S.amount*S.pricePerUnit - P.amount*P.productionCost >= T.second " +
                "UNION " +
                "SELECT DISTINCT P.factoryId, Pr.productId, Pr.productName, Pr.productType, - P.amount*P.productionCost AS profit " +
                "FROM Product Pr, Produce P, " +
                "(" +
                "SELECT Pr.productId AS first, MAX(-P.amount*P.productionCost) AS second  " +
                "FROM Product Pr, Produce P " +
                "WHERE Pr.productId = P.productId AND ( P.productId) NOT IN ( SELECT S.productId FROM Shipment S ) " +
                "GROUP BY Pr.productId " +
                ") T " +
                "WHERE Pr.productId = P.productId AND T.first = P.productId   AND  (-P.amount*P.productionCost) >= T.second " +
                "ORDER BY profit DESC, productId ASC ";

        String query1 = "SELECT DISTINCT P.factoryId, Pr.productId, Pr.productName, Pr.productType, S.amount*S.pricePerUnit - P.amount*P.productionCost AS profit " +
                "FROM Product Pr, Produce P, Shipment S, " +
                "(" +
                "SELECT V.first, MAX(V.second) AS second " +
                "FROM " +
                "(" +
                "SELECT Pr.productId AS first , S.amount*S.pricePerUnit - P.amount*P.productionCost AS second " +
                "FROM Product Pr, Produce P, Shipment S " +
                "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND P.productId = S.productId  " +
                "UNION " +
                "SELECT Pr.productId AS first, -P.amount*P.productionCost AS second " +
                "FROM Product Pr, Produce P " +
                "WHERE Pr.productId = P.productId AND Pr.productId NOT IN ( SELECT S.productId FROM Shipment S  ) " +
                ") V " +
                "GROUP BY V.first " +
                ") T " +
                "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND Pr.productId = S.productId AND T.first = Pr.productId AND " +
                " S.amount*S.pricePerUnit - P.amount*P.productionCost >= T.second " +
                "ORDER BY profit DESC, Pr.productId ASC";
        String query = "SELECT DISTINCT P.factoryId, Pr.productId, Pr.productName, Pr.productType, S.amount*S.pricePerUnit - P.amount*P.productionCost AS profit " +
                            "FROM Product Pr, Produce P, Shipment S, " +
                            "(" +
                            "SELECT Pr.productId AS first, MAX(S.amount*S.pricePerUnit - P.amount*P.productionCost) AS second " +
                            "FROM Product Pr, Produce P, Shipment S " +
                            "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND Pr.productId = S.productId " +
                            "GROUP BY Pr.productId" +
                            ") T " +
                            "WHERE Pr.productId = P.productId AND P.factoryId = S.factoryId AND Pr.productId = S.productId AND T.first = Pr.productId AND " +
                            " S.amount*S.pricePerUnit - P.amount*P.productionCost >= T.second " +
                            "ORDER BY profit DESC, Pr.productId ASC";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query3);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            java.util.ArrayList<QueryResult.MostValueableProduct> al = new java.util.ArrayList<>();
            while(res.next()){
                QueryResult.MostValueableProduct mvp = new  QueryResult.MostValueableProduct(res.getInt("factoryId"),
                        res.getInt("productId"),
                        res.getString("productName"),
                        res.getString("productType"),
                        res.getDouble("profit"));
                al.add(mvp);
            }
            int size = al.size();
            QueryResult.MostValueableProduct[] ans = new QueryResult.MostValueableProduct[size];
            int i = 0;
            for(QueryResult.MostValueableProduct mvp : al){
                ans[i] = mvp;
                i++;
            }
            return ans;
        }
        catch( java.sql.SQLException e){
            e.printStackTrace();
        }
        return new QueryResult.MostValueableProduct[0];
    }


    /**
     * For each department, should return all employees that are paid under the
     *     average salary for that department. You consider the employees
     *     that do not work earning ”0”.
     *
     * @return QueryResult.LowSalaryEmployees[]
     */
    public QueryResult.LowSalaryEmployees[] getLowSalaryEmployeesForDepartments() {
        //System.out.println("CALLED  getLowSalaryEmployeesForDepartments");
        String query2 = "SELECT DISTINCT E.employeeId, E.employeeName, E.department, E.salary " +
                "FROM " +
                "(" +
                "SELECT E.* " +
                "FROM Employee E " +
                "WHERE EXISTS ( SELECT * FROM Works_In W WHERE W.employeeId = E.employeeId ) " +
                "UNION " +
                "SELECT E.employeeId, E.employeeName, E.department, 0 AS salary " +
                "FROM Employee E " +
                "WHERE NOT EXISTS ( SELECT * FROM Works_In W WHERE W.employeeId = E.employeeId ) " +
                ") E " +
                ", " +
                "( " +
                "SELECT Azd.department, AVG(salary) as avg " +
                "FROM ( " +
                "    SELECT E.* " +
                "    FROM Employee E " +
                "    WHERE EXISTS (SELECT * FROM Works_In W WHERE W.employeeId = E.employeeId ) " +
                "    UNION " +
                "    SELECT E.employeeId, E.employeeName, E.department, 0 AS salary " +
                "    FROM Employee E " +
                "    WHERE NOT EXISTS (SELECT * FROM Works_In W WHERE W.employeeId = E.employeeId ) " +
                "    ) AS Azd " +
                "GROUP BY Azd.department " +
                ")  T " +
                "WHERE E.department = T.department AND E.salary < T.avg " +
                " ORDER BY E.employeeId ASC ";
        String query1 =
                        "SELECT DISTINCT E.* " +
                        "FROM Employee E," +
                        "(" +
                        "SELECT T.department , AVG(T.salary) AS salary " +
                        "FROM " +
                                "(" +
                                "SELECT * " +
                                "FROM Employee E " +
                                "WHERE EXISTS ( SELECT * FROM Works_In  W WHERE W.employeeId = E.employeeId ) " +
                                ") T " +
                        "GROUP BY T.department" +
                        ") T " +
                        "WHERE E.department = T.department AND E.salary < T.salary " +
                        "  ORDER BY E.employeeId ASC ";

        String query = "SELECT DISTINCT E.employeeId, E.employeeName, E.department, E.salary " +
                "FROM Employee E, " +
                "( " +
                "SELECT Azd.department, AVG(salary) as avg " +
                "FROM ( " +
                "    SELECT E.* " +
                "    FROM Employee E " +
                "    WHERE EXISTS (SELECT * FROM Works_In W WHERE W.employeeId = E.employeeId ) " +
                "    UNION " +
                "    SELECT E.employeeId, E.employeeName, E.department, 0 AS salary " +
                "    FROM Employee E " +
                "    WHERE NOT EXISTS (SELECT * FROM Works_In W WHERE W.employeeId = E.employeeId ) " +
                "    ) AS Azd " +
                "GROUP BY Azd.department " +
                ")  T " +
                "WHERE E.department = T.department AND E.salary < T.avg " +
                " ORDER BY E.employeeId ASC ";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query2);
            java.sql.ResultSet res = preparedStatement.executeQuery();
            java.util.ArrayList<QueryResult.LowSalaryEmployees> al = new java.util.ArrayList<>();
            while(res.next()){
                QueryResult.LowSalaryEmployees mvp = new  QueryResult.LowSalaryEmployees(res.getInt("employeeId"),
                        res.getString("employeeName"),
                        res.getString("department"),
                        res.getInt("salary"));
                al.add(mvp);
            }
            int size = al.size();
            QueryResult.LowSalaryEmployees[] ans = new QueryResult.LowSalaryEmployees[size];
            int i = 0;
            for(QueryResult.LowSalaryEmployees mvp : al){
                ans[i] = mvp;
                i++;
            }
            return ans;
        }
        catch( java.sql.SQLException e){
            e.printStackTrace();
        }
        return new QueryResult.LowSalaryEmployees[0];
    }



    /**
     * For the products of given productType, increase the productionCost of every unit by a given percentage.
     *
     * @return number of rows affected
     */
    public int increaseCost(String productType, double percentage) {
        //System.out.println("CALLED  increaseCost");
        String query = "UPDATE Produce P " +
                "SET  productionCost = productionCost * (? + 1 ) " +
                " WHERE EXISTS (SELECT * FROM Product Pr WHERE P.productId = Pr.productId AND Pr.productType = ?"  +
                ")";
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query);
            preparedStatement.setDouble(1, percentage);
            preparedStatement.setString(2, productType);
            int result = preparedStatement.executeUpdate();
            return result;
        }
        catch(java.sql.SQLException e){
            e.printStackTrace();
        }
        return 0;
    }


    /**
     * Deleting all employees that have not worked since the given date.
     *
     * @return number of rows affected
     */
    public int deleteNotWorkingEmployees(String givenDate) {
        //System.out.println("CALLED  deleteNotWorkingEmployees");
        String query = "DELETE FROM Employee WHERE employeeId NOT IN ( SELECT employeeId FROM Works_In WHERE startDate >= ? ) " ;
        try{
            java.sql.PreparedStatement preparedStatement = connection.prepareStatement(query);
            preparedStatement.setString(1, givenDate);
            int result = preparedStatement.executeUpdate();
            return result;
        }
        catch(java.sql.SQLException e){
            e.printStackTrace();
        }
        return 0;
    }

    /**
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     *  THE METHODS AFTER THIS LINE WILL NOT BE GRADED.
     *  YOU DON'T HAVE TO SOLVE THEM, LEAVE THEM AS IS IF YOU WANT.
     *  IF YOU HAVE ANY QUESTIONS, REACH ME VIA EMAIL.
     * *****************************************************
     * *****************************************************
     * *****************************************************
     * *****************************************************
     */

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Peers are considered tied and receive the same rank. After
     * that, there should be a gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each department, find the rank of the employees in terms of
     * salary. Everything is the same but after ties, there should be no
     * gap.
     *
     * @return QueryResult.EmployeeRank[]
     */
    public QueryResult.EmployeeRank[] calculateRank2() {
        return new QueryResult.EmployeeRank[0];
    }

    /**
     * For each factory, calculate the most profitable 4th product.
     *
     * @return QueryResult.FactoryProfit
     */
    public QueryResult.FactoryProfit calculateFourth() {
        return new QueryResult.FactoryProfit(0,0,0);
    }

    /**
     * Determine the salary variance between an employee and another
     * one who began working immediately after the first employee (by
     * startDate), for all employees.
     *
     * @return QueryResult.SalaryVariant[]
     */
    public QueryResult.SalaryVariant[] calculateVariance() {
        return new QueryResult.SalaryVariant[0];
    }

    /**
     * Create a method that is called once and whenever a Product starts
     * losing money, deletes it from Produce table
     *
     * @return void
     */
    public void deleteLosing() {

    }
}
