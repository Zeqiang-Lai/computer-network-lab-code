package lab4;

public class Frame {
	private String kind;
	private int seq;
	private int ack;
	private String info;
	public String getKind() {
		return kind;
	}
	public void setKind(String kind) {
		this.kind = kind;
	}
	public int getSeq() {
		return seq;
	}
	public void setSeq(int seq) {
		this.seq = seq;
	}
	public int getAck() {
		return ack;
	}
	public void setAck(int ack) {
		this.ack = ack;
	}
	public String getInfo() {
		return info;
	}
	public void setInfo(String info) {
		this.info = info;
	}
	public void setInfo(Packet p) {
		this.info = p.getData();
	}
	
	
}
