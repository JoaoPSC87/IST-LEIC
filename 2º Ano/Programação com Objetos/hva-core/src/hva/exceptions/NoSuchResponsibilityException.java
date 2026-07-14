package hva.exceptions;

public class NoSuchResponsibilityException extends Exception {
    private final String _employeeId;
    private final String _reponsibilityId;
    
    public NoSuchResponsibilityException(String employeeId, String responsibilityId) {
        _employeeId = employeeId;
        _reponsibilityId = responsibilityId;
    }

    public String getEmployeeID() {
        return _employeeId;
    }

    public String getResponsibilityID() {
        return _reponsibilityId;
    }
}
